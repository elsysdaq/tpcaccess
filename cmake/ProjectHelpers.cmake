include(cmake/CompilerSettings.cmake)

# ============================================================================
# Helper Functions
# ============================================================================
# Removes one compile option from the current directory after it was added with
# add_compile_options(). The option must match exactly, including generator
# expressions.
function(remove_compile_option OPTION_TO_REMOVE)
    get_directory_property(TPC_COMPILE_OPTIONS COMPILE_OPTIONS)
    if(TPC_COMPILE_OPTIONS)
        list(REMOVE_ITEM TPC_COMPILE_OPTIONS "${OPTION_TO_REMOVE}")
        set_directory_properties(PROPERTIES COMPILE_OPTIONS "${TPC_COMPILE_OPTIONS}")
    endif()
endfunction()

# Function to create a tpcserver compilation target with some defaults
# Usage:
#   add_tpc_project(target_name)  # Auto-discover sources
#   add_tpc_project(target_name SOURCES source1.cpp source2.cpp ...)  # Use provided sources
function(add_tpc_project TARGET_NAME)
    add_tpc_project_properties(${TARGET_NAME})
    add_tpc_project_sources(${TARGET_NAME} ${ARGN})
endfunction()

function(add_tpc_project_sources TARGET_NAME)
    # Parse optional SOURCES argument
    cmake_parse_arguments(LIB "" "" "SOURCES" ${ARGN})

    # Get directory where this function is called from
    set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    # Use provided sources or auto-discover
    if(LIB_SOURCES)
        # Use provided sources list
        set(FINAL_SOURCES ${LIB_SOURCES})
    else()
        # Automatically discover source files
        file(GLOB_RECURSE FINAL_SOURCES
            CONFIGURE_DEPENDS
            "${TARGET_DIR}/*.cpp"
            "${TARGET_DIR}/*.c"
        )
    endif()

    # Always auto-discover headers
    file(GLOB_RECURSE LIB_HEADERS
        CONFIGURE_DEPENDS
        "${TARGET_DIR}/*.h"
        "${TARGET_DIR}/*.hpp"
    )

    # Add sources
    target_sources(${TARGET_NAME}
        PRIVATE
            ${FINAL_SOURCES}
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${TARGET_DIR}
            FILES
                ${LIB_HEADERS}
    )

    list(LENGTH FINAL_SOURCES SOURCE_COUNT)
    message(STATUS "  ${TARGET_NAME}: added ${SOURCE_COUNT} source files")
endfunction()

function(add_tpc_project_properties TARGET_NAME)
    set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    # Include directories
    target_include_directories(${TARGET_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${TARGET_DIR}>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
            $<INSTALL_INTERFACE:include>
    )

    # Set project-default compiler warning settings
    target_link_libraries(${TARGET_NAME}
        PRIVATE
			$<BUILD_LOCAL_INTERFACE:tpc_compiler_warnings>
    )

    # Get relative path from repository root to target project
    file(RELATIVE_PATH relative ${PROJECT_SOURCE_DIR} ${TARGET_DIR}) 
    cmake_path(GET relative PARENT_PATH PARENT_DIR)

    set_target_properties(${TARGET_NAME}
        PROPERTIES
            PREFIX ""             # Disable automatic "lib" prefix for all targets
            FOLDER ${PARENT_DIR}  # Used by VS for folder structure in CMake Targets view
    )

    message(STATUS "Adding target ${TARGET_NAME} (${TARGET_DIR})")
endfunction()
