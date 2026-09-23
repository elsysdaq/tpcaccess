# Read version metadata for a project key from the JSON metadata file.
# Exposes variables like COMMON_MAJOR, COMMON_MINOR, COMMON_PATCH, COMMON_VERSION_STR.
macro(get_version_metadata input_file)
    set(options "")
    set(oneValueArgs PROJECT)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Default to the top-level "project" key if no project name is supplied.
    if(ARG_PROJECT)
        set(key ${ARG_PROJECT})
    else()
        set(key "project")
    endif()

    string(TOUPPER ${key} var_prefix)

    file(READ ${input_file} versions_json)

    # Extract semantic version components from the selected JSON object.
    string(JSON version_info GET ${versions_json} ${key})
    string(JSON ${var_prefix}_MAJOR GET ${version_info} "major")
    string(JSON ${var_prefix}_MINOR GET ${version_info} "minor")
    string(JSON ${var_prefix}_PATCH GET ${version_info} "patch")

    set(${var_prefix}_VERSION_STR "${${var_prefix}_MAJOR}.${${var_prefix}_MINOR}.${${var_prefix}_PATCH}")
endmacro()

function(detect_working_directory_kind out_var working_dir)
    set(_kind "none")

    if(NOT IS_DIRECTORY "${working_dir}")
        set(${out_var} "${_kind}" PARENT_SCOPE)
        return()
    endif()

    find_program(_DWK_SVN_EXECUTABLE NAMES svn)
    find_program(_DWK_GIT_EXECUTABLE NAMES git)

    # Native SVN working copy
    if(_DWK_SVN_EXECUTABLE)
        execute_process(
            COMMAND "${_DWK_SVN_EXECUTABLE}" info "${working_dir}"
            RESULT_VARIABLE _svn_result
            OUTPUT_QUIET
            ERROR_QUIET
        )

        if(_svn_result EQUAL 0)
            set(${out_var} "svn" PARENT_SCOPE)
            return()
        endif()
    endif()

    # Git or git-svn working tree
    if(_DWK_GIT_EXECUTABLE)
        execute_process(
            COMMAND "${_DWK_GIT_EXECUTABLE}" -C "${working_dir}" rev-parse --is-inside-work-tree
            RESULT_VARIABLE _git_result
            OUTPUT_VARIABLE _git_is_work_tree
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(_git_result EQUAL 0 AND _git_is_work_tree STREQUAL "true")
            execute_process(
                COMMAND "${_DWK_GIT_EXECUTABLE}" -C "${working_dir}"
                        config --get-regexp "^svn-remote\\..*\\.url$"
                RESULT_VARIABLE _git_svn_result
                OUTPUT_QUIET
                ERROR_QUIET
            )

            if(_git_svn_result EQUAL 0)
                set(${out_var} "git svn" PARENT_SCOPE)
            else()
                set(${out_var} "git" PARENT_SCOPE)
            endif()

            return()
        endif()
    endif()

    set(${out_var} "${_kind}" PARENT_SCOPE)
endfunction()

# Determine a source-control revision id.
# Prefer SVN revision numbers, then git-svn revisions, then a normal Git short hash.
function(get_revision_id out_var)
    execute_process(
        COMMAND svnversion
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE svn_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE svn_result
    )

    if(svn_result EQUAL 0
       AND svn_output
       AND NOT svn_output STREQUAL "Unversioned directory"
       AND svn_output MATCHES "([0-9]+)")
        set(${out_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        return()
    endif()

    execute_process(
        COMMAND git svn find-rev git-svn
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE git_svn_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE git_svn_result
    )

    if(git_svn_result EQUAL 0 AND git_svn_output MATCHES "^[0-9]+$")
        set(${out_var} "${git_svn_output}" PARENT_SCOPE)
        return()
    endif()

    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE git_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE git_result
    )

    if(git_result EQUAL 0 AND git_output)
        set(${out_var} "${git_output}" PARENT_SCOPE)
        return()
    endif()

    set(${out_var} "unknown" PARENT_SCOPE)

    if(ENABLE_REVISION_ID_IN_VERSION)
        message(STATUS "Unable to determine project revision")
        message(FATAL_ERROR "Project build settings require a revision id (set ENABLE_REVISION_ID_IN_VERSION to OFF)")
    endif()
endfunction()

function(get_svn_revision_id out_var working_dir)
    find_program(SVN_VERSION_EXECUTABLE NAMES svnversion REQUIRED)

    execute_process(
        COMMAND "${SVN_VERSION_EXECUTABLE}"
        WORKING_DIRECTORY "${working_dir}"
        OUTPUT_VARIABLE svn_output
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )

    if(NOT svn_output MATCHES "([0-9]+)")
        message(FATAL_ERROR "Could not parse SVN revision")
    endif()

    set(${out_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()


function(get_git_svn_revision_id out_var working_dir)
    find_program(GIT_EXECUTABLE NAMES git REQUIRED)

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" -C "${working_dir}" svn find-rev git-svn
        OUTPUT_VARIABLE git_svn_output
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )

    if(NOT git_svn_output MATCHES "^[0-9]+$")
        message(FATAL_ERROR "Could not parse git-svn revision")
    endif()

    set(${out_var} "${git_svn_output}" PARENT_SCOPE)
endfunction()


function(get_git_revision_id out_var working_dir)
    find_program(GIT_EXECUTABLE NAMES git REQUIRED)

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" -C "${working_dir}" rev-parse --short HEAD
        OUTPUT_VARIABLE git_output
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )

    if(NOT git_output)
        message(FATAL_ERROR "Could not determine Git revision")
    endif()

    set(${out_var} "${git_output}" PARENT_SCOPE)
endfunction()

function(hex_to_decimal hex_string out_var)
    string(TOUPPER "${hex_string}" upper)
    set(hex_chars "0123456789ABCDEF")
    set(result 0)
    string(LENGTH "${upper}" len)
    math(EXPR last "(${len} - 1)")
    foreach(i RANGE 0 ${last})
        string(SUBSTRING "${upper}" ${i} 1 ch)
        string(FIND "${hex_chars}" "${ch}" pos)
        math(EXPR "result = ${result} * 16 + ${pos}")
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()


# Compute 16 bit hash with truncated SHA-1
function(compute_hash16 input out_var)
    string(SHA1 sha1_full "${input}")
    string(SUBSTRING "${sha1_full}" 0 4 hash_hex)  # first 4 hex digits = 16 bits
    hex_to_decimal("${hash_hex}" hash_dec)
    set(${out_var} "${hash_dec}" PARENT_SCOPE)
endfunction()

function(set_project_revision_id)
    detect_working_directory_kind(repo_type "${CMAKE_SOURCE_DIR}")

    if(repo_type STREQUAL "svn")
        get_svn_revision_id(rev_id "${CMAKE_SOURCE_DIR}")
        set_property(GLOBAL PROPERTY revision_rc_id "${rev_id}")
    elseif(repo_type STREQUAL "git svn")
        get_git_svn_revision_id(rev_id "${CMAKE_SOURCE_DIR}")
        set_property(GLOBAL PROPERTY revision_rc_id "${rev_id}")
    elseif(repo_type STREQUAL "git")
        get_git_revision_id(rev_id "${CMAKE_SOURCE_DIR}")
        compute_hash16(${rev_id} rev_rc_id)
        set_property(GLOBAL PROPERTY revision_rc_id "${rev_rc_id}")
    elseif(repo_type STREQUAL "none")
        set(rev_id "unknown")
        if(ENABLE_REVISION_ID_IN_VERSION)
            message(FATAL_ERROR "Project build settings require a revision id (set ENABLE_REVISION_ID_IN_VERSION to OFF)")
        endif()
    else()
        message(FATAL_ERROR "Unknown repository type: ${repo_type}")
    endif()

    set_property(GLOBAL PROPERTY revision_id "${rev_id}")
    message(STATUS "Found project revision ${rev_id}")
endfunction()

# Generate a version header for a specific project using version.h.in.
function(generate_version_header output_file project)
    get_version_metadata("${PROJECT_SOURCE_DIR}/build/version_metadata.json" PROJECT ${project})

    string(TOUPPER ${project} var_prefix)

    # Variables consumed by version.h.in.
    set(PREFIX ${var_prefix})
    set(VERSION_MAJOR ${${var_prefix}_MAJOR})
    set(VERSION_MINOR ${${var_prefix}_MINOR})
    set(VERSION_PATCH ${${var_prefix}_PATCH})

    if(ENABLE_REVISION_ID_IN_VERSION)
        get_property(rev_id GLOBAL PROPERTY revision_id)
        get_property(rev_rc_id GLOBAL PROPERTY revision_rc_id)
        set(VERSION_SUFFIX ${rev_id})
        set(VERSION_RC_VAL ${rev_rc_id})
    else()
        set(VERSION_SUFFIX ${DEFAULT_VERSION_SUFFIX})
        set(VERSION_RC_VAL 0)
    endif()

    configure_file(${PROJECT_SOURCE_DIR}/build/version.h.in ${output_file})
    message(STATUS "Generated ${output_file} with ${project} version ${${var_prefix}_VERSION_STR}")
endfunction()

# Generates the version_info file used by CI
function(generate_version_info output_file project)
    get_version_metadata("${PROJECT_SOURCE_DIR}/build/version_metadata.json" PROJECT ${project})

    string(TOUPPER ${project} var_prefix)

    set(PREFIX ${var_prefix})
    set(VERSION_MAJOR ${${var_prefix}_MAJOR})
    set(VERSION_MINOR ${${var_prefix}_MINOR})
    set(VERSION_PATCH ${${var_prefix}_PATCH})

	get_property(rev_id GLOBAL PROPERTY revision_id)

    set(REVISION_ID ${rev_id})
    if(ENABLE_REVISION_ID_IN_VERSION)
        set(VERSION_SUFFIX ${rev_id})
    else()
        set(VERSION_SUFFIX ${DEFAULT_VERSION_SUFFIX})
    endif()

    configure_file(${PROJECT_SOURCE_DIR}/build/version_info.in ${output_file})
    message(STATUS "Generated ${output_file} with version ${${var_prefix}_VERSION_STR}")
endfunction()
