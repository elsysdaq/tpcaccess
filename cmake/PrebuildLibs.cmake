# PrebuildExtern.cmake
# Utilities for prebuilding external libraries (Xerces-C++, HDF5, etc.)
#
# This module provides functions to automate copying built external libraries
# to prebuilt directories for faster subsequent builds.
#
# Configuration Options:
#   - DISABLE_PREBUILT (default: OFF) - Forces all external libraries to build from source
#   - USE_PREBUILT_XERCES (default: ON) - Use prebuilt Xerces-C++ binaries
#   - Additional USE_PREBUILT_* options can be added for other libraries
#
# Usage:
#   1. Include this file: include(build/PrebuildExtern.cmake)
#   2. Call add_prebuild_library() for each library after add_subdirectory()
#   3. Call add_prebuild_file() for each additional file to copy
#   4. Build with: cmake --build <build-dir> --target prebuild-extern
#
# Example:
#   include(build/PrebuildExtern.cmake)
#
#   if(USE_PREBUILT_XERCES)
#       add_subdirectory(extern/xerces-c-prebuilt)
#   else()
#       add_subdirectory(extern/xerces-c)
#
#       add_prebuild_library(xerces-c
#           LIBRARY_DIR extern/xerces-c-prebuilt/bin/${PLATFORM_ARCH}
#       )
#
#       add_prebuild_file(xerces-c
#           SOURCE extern/xerces-c/src/xercesc/util/XercesVersion.hpp
#           DESTINATION extern/xerces-c-prebuilt/include/${PLATFORM_ARCH}/xercesc/util/XercesVersion.hpp
#       )
#   endif()
#
# Regenerating prebuilts:
#   cmake --preset x86_64-windows -DDISABLE_PREBUILT=ON
#   cmake --build out/x86_64-windows --target prebuild-extern --config Debug
#   cmake --build out/x86_64-windows --target prebuild-extern --config Release

cmake_minimum_required(VERSION 3.25)

include_guard(GLOBAL)

# Create main prebuild-extern target (empty, libraries will add themselves as dependencies)
if(NOT TARGET prebuild-all)
    add_custom_target(prebuild-all
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "=========================================="
        COMMAND ${CMAKE_COMMAND} -E echo "Prebuild External Libraries"
        COMMAND ${CMAKE_COMMAND} -E echo "=========================================="
        COMMAND ${CMAKE_COMMAND} -E echo "This target builds external libraries from source and copies"
        COMMAND ${CMAKE_COMMAND} -E echo "them to the prebuilt directories for faster subsequent builds."
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "If no libraries are registered (using prebuilts):"
        COMMAND ${CMAKE_COMMAND} -E echo "  Reconfigure with -DDISABLE_PREBUILT=ON to rebuild prebuilts"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMENT "Prebuilding external libraries from source..."
        VERBATIM
    )
    set_target_properties(prebuild-all PROPERTIES FOLDER "prebuild")

    # Add a variable to track if any libraries were registered
    set_property(GLOBAL PROPERTY PREBUILD_LIBRARIES_REGISTERED FALSE)
endif()

#------------------------------------------------------------------------------
# use_prebuild_library
#
# Workaround because IMPORTED_LOCATION properties do not support generator
# expressions. By copying to a fixed path we can then specify a fixed
# imported location property.
#
# Parameters:
#   target_name   - Name of the imported library target (e.g., xerces-c)
#   library_dir   - Source directory containing prebuilt files, relative to
#                   PROJECT_SOURCE_DIR. May use generator expressions.
#
# Example:
#    use_prebuild_library(xerces-c extern/xerces-c-prebuilt/bin/${PRESET_FULL_NAME}
#        LIBRARY_LOCATION_DEBUG xerces-c_3D.lib
#        LIBRARY_LOCATION_RELEASE xerces-c_3.lib
#    )
#------------------------------------------------------------------------------
function(use_prebuild_library target_name library_dir)
    if(IS_ABSOLUTE "${library_dir}")
        set(source_dir "${library_dir}")
    else()
        set(source_dir "${PROJECT_SOURCE_DIR}/${library_dir}")
    endif()

	set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/prebuild/${target_name}")

    set(_args ${ARGN}) 
    set(_props)
    set(_configs)

    # Small argument parser
	while(_args)
		list(POP_FRONT _args _key)

		if(NOT _key MATCHES "^LIBRARY_LOCATION(_[A-Za-z0-9_]+)?$")
			message(FATAL_ERROR "func: unknown argument '${_key}'")
		endif()

		list(LENGTH _args _remaining)
		if(_remaining EQUAL 0)
			message(FATAL_ERROR "func: '${_key}' requires a value")
		endif()

		list(POP_FRONT _args _value)

		string(REGEX REPLACE "^LIBRARY_LOCATION" "IMPORTED_LOCATION" _prop "${_key}")
		list(APPEND _props "${_prop}" "${output_dir}/${_value}")

		if(_key MATCHES "^LIBRARY_LOCATION_(.+)$")
			list(APPEND _configs "${CMAKE_MATCH_1}")
		endif()
	endwhile()

	if(_configs)
		list(REMOVE_DUPLICATES _configs)
		set_target_properties("${target_name}" PROPERTIES
			IMPORTED_CONFIGURATIONS "${_configs}"
		)
	endif()

	set_target_properties("${target_name}" PROPERTIES
	    ${_props}
	)

    set(copy_target_name "use-prebuild-${target_name}")
    if(NOT TARGET ${copy_target_name})
        add_custom_target(${copy_target_name}
            COMMAND ${CMAKE_COMMAND} -E echo "Copying prebuilt ${target_name}..."
            COMMAND ${CMAKE_COMMAND} -E make_directory "${output_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${source_dir}"
                "${output_dir}"
            COMMENT "Preparing prebuilt ${target_name} ($<CONFIG>)"
            VERBATIM
        )
        set_target_properties(${copy_target_name} PROPERTIES FOLDER "prebuild")
    endif()

    add_dependencies(${target_name} ${copy_target_name})

    message(STATUS "Using ${target_name} prebuilt library from ${library_dir}")
endfunction()

#------------------------------------------------------------------------------
# add_prebuild_library
#
# Registers an external library for prebuilding. Creates a custom target that:
# 1. Depends on the library's build target
# 2. Copies the built library to the prebuilt directory
#
# Parameters:
#   TARGET          - Name of the library target (e.g., xerces-c)
#   LIBRARY_DIR     - Destination directory for library files (relative to PROJECT_SOURCE_DIR)
#                     Can use CMake variables like ${PLATFORM_ARCH}
#
# Notes:
#   - Automatically handles multi-config generators (MSVC) vs single-config (Make/Ninja)
#   - On Windows: Libraries copied to LIBRARY_DIR/ with config-specific names
#   - On Linux: Libraries copied to LIBRARY_DIR_Debug/ and LIBRARY_DIR_Release/
#   - PDB files are automatically copied on Windows when available
#
# Use add_prebuild_file() to copy additional files (headers, configs, etc.)
#------------------------------------------------------------------------------
function(add_prebuild_library target_name)
    set(options ADD_PDB)
    set(oneValueArgs LIBRARY_DIR)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Validate arguments
    if(NOT ARG_LIBRARY_DIR)
        message(FATAL_ERROR "add_prebuild_library: LIBRARY_DIR is required")
    endif()

    # Create subtarget for this library
    set(subtarget_name prebuild-${target_name})

    if(NOT TARGET ${subtarget_name})
        add_custom_target(${subtarget_name}
            COMMAND ${CMAKE_COMMAND} -E echo ">>> Processing ${target_name}..."
            COMMENT "Prebuilding ${target_name}..."
            VERBATIM
        )

        # Make it depend on the actual library build
        add_dependencies(${subtarget_name} ${target_name})

        # Add to main prebuild-extern target
        add_dependencies(prebuild-all ${subtarget_name})

        # Organize in IDE
        set_target_properties(${subtarget_name} PROPERTIES FOLDER "prebuild")

        # Mark that at least one library was registered
        set_property(GLOBAL PROPERTY PREBUILD_LIBRARIES_REGISTERED TRUE)
    endif()

    #--------------------------------------------------------------------------
    # Copy library files
    #--------------------------------------------------------------------------

    get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

    if(is_multi_config)
        add_custom_command(TARGET ${subtarget_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying $<CONFIG> library..."
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "${PROJECT_SOURCE_DIR}/${ARG_LIBRARY_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${target_name}>"
                "${PROJECT_SOURCE_DIR}/${ARG_LIBRARY_DIR}/"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"$<TARGET_LINKER_FILE:${target_name}>"
				"${PROJECT_SOURCE_DIR}/${ARG_LIBRARY_DIR}/"
            VERBATIM
        )
    else()
        set(output_dir "${PROJECT_SOURCE_DIR}/${ARG_LIBRARY_DIR}_$<CONFIG>")

        add_custom_command(TARGET ${subtarget_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying $<CONFIG> library..."
            COMMAND ${CMAKE_COMMAND} -E make_directory "${output_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${target_name}>"
                "${output_dir}/"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"$<TARGET_LINKER_FILE:${target_name}>"
				"${output_dir}/"
            VERBATIM
        )
    endif()

    # Deal with pdb files too
	if(MSVC AND ADD_PDB)
		set(_pdb_file "$<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_BASE_NAME:${target_name}>.pdb")
		set(_idb_file "$<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_BASE_NAME:${target_name}>.idb")
        add_custom_command(TARGET ${subtarget_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying pdb/idb files if present ..."
			COMMAND ${CMAKE_COMMAND}
				"-DSRC=${_pdb_file}"
				"-DDST_DIR=${PROJECT_SOURCE_DIR}/${ARG_LIBRARY_DIR}"
				-P "${PROJECT_SOURCE_DIR}/cmake/CopyIfExists.cmake"
			COMMAND ${CMAKE_COMMAND}
				"-DSRC=${_idb_file}"
				"-DDST_DIR=${PROJECT_SOURCE_DIR}/${ARG_LIBRARY_DIR}"
				-P "${PROJECT_SOURCE_DIR}/cmake/CopyIfExists.cmake"
			VERBATIM
		)
	endif()

    message(STATUS "Registered ${target_name} for prebuilding")
    message(STATUS "  Library output: ${ARG_LIBRARY_DIR}")

endfunction()

#------------------------------------------------------------------------------
# add_prebuild_file
#
# Adds a file to copy during prebuilding. Attaches to the prebuild target
# for the specified library.
#
# Parameters:
#   TARGET          - Name of the library target (must have called add_prebuild_library first)
#   SOURCE          - Source file path (relative to CMAKE_BINARY_DIR)
#                     Can use generator expressions like $<CONFIG>
#   DESTINATION     - Destination file path (relative to PROJECT_SOURCE_DIR)
#                     Can use CMake variables and generator expressions
#
# Example:
#   add_prebuild_file(xerces-c
#       SOURCE extern/xerces-c/src/xercesc/util/XercesVersion.hpp
#       DESTINATION extern/xerces-c-prebuilt/include/${PLATFORM_ARCH}/xercesc/util/XercesVersion.hpp
#   )
#------------------------------------------------------------------------------
function(add_prebuild_file target_name)
    set(options "")
    set(oneValueArgs SOURCE DESTINATION)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Validate arguments
    if(NOT ARG_SOURCE)
        message(FATAL_ERROR "add_prebuild_file: SOURCE is required")
    endif()
    if(NOT ARG_DESTINATION)
        message(FATAL_ERROR "add_prebuild_file: DESTINATION is required")
    endif()

    # Check that the prebuild target exists
    set(subtarget_name prebuild-${target_name})
    if(NOT TARGET ${subtarget_name})
        message(FATAL_ERROR "add_prebuild_file: Must call add_prebuild_library(${target_name}) first")
    endif()

    # Construct full paths
    set(source_path "${CMAKE_BINARY_DIR}/${ARG_SOURCE}")
    set(dest_path "${PROJECT_SOURCE_DIR}/${ARG_DESTINATION}")

    # Get destination directory
    get_filename_component(dest_dir "${dest_path}" DIRECTORY)
    get_filename_component(filename "${dest_path}" NAME)

    add_custom_command(TARGET ${subtarget_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying file: ${filename}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${dest_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${source_path}"
            "${dest_path}"
        VERBATIM
    )

endfunction()
