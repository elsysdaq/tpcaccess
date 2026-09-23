cmake_minimum_required(VERSION 3.26)

option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_PYTHON_WRAPPERS "Build python wrappers (requires SWIG)" OFF)
option(ENABLE_BUILD_FLAG_DEFAULTS "Enables a set of default compilation options on all targets" OFF)
option(ENABLE_SANITIZER_DEFAULTS "Enable sanitizers in supported debug build configurations" OFF)
option(EXAMPLES_USE_PREBUILT_TPCACCESS "Use prebuilt tpcaccess binaries" OFF)
option(EXAMPLES_USE_STATIC_TPCACCESS "Use tpcaccess-static for building the examples" ON)

option(ENABLE_INSTALL "Generate the install target" ON)
option(INSTALL_TPCACCESS "Install tpcaccess library" ON)
option(INSTALL_PYTPCACCESS "Install python bindings for tpcaccess" ON)
set(PYTPCACCESS_INSTALL_DIR "tpcaccess" CACHE PATH
    "Install directory for the tpcaccess python wrapper, relative to the installation prefix")

include(cmake/Version.cmake)
get_version_metadata("${CMAKE_SOURCE_DIR}/build/version_metadata.json" PROJECT "tpcaccess")

# Project declaration
project(TpcAccess
    VERSION ${TPCACCESS_VERSION_STR}
    DESCRIPTION "TpcAccess API for Elsys DAQ devices"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# CCache Support
include(cmake/CCache.cmake)

# Helper functions for setting target properties
include(cmake/ProjectHelpers.cmake)

# Predefined compiler options and flags
include(cmake/CompilerSettings.cmake)

# Prebuild targets
include(cmake/PrebuildLibs.cmake)

if(MSVC)
    set(xmlch-type "wchar_t" CACHE STRING "XMLCh type" FORCE)
endif()

function(add_static_xerces)
    set(_had_cached_value FALSE)

    if(DEFINED CACHE{BUILD_SHARED_LIBS})
        set(_had_cached_value TRUE)
        get_property(
            _saved_value
            CACHE BUILD_SHARED_LIBS
            PROPERTY VALUE
        )
    endif()

    # The cache value prevents Xerces from replacing it with its ON default.
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    # Also shadow any normal variable inherited from the caller.
    set(BUILD_SHARED_LIBS OFF)

    add_subdirectory(extern/xerces-c EXCLUDE_FROM_ALL)

    get_target_property(_xerces_type xerces-c TYPE)
    if(NOT _xerces_type STREQUAL "STATIC_LIBRARY")
        message(FATAL_ERROR "Expected static Xerces, got ${_xerces_type}")
    endif()

    if(_had_cached_value)
        set(
            BUILD_SHARED_LIBS "${_saved_value}"
            CACHE BOOL "Build shared libraries" FORCE
        )
    else()
        unset(BUILD_SHARED_LIBS CACHE)
    endif()
endfunction()

add_static_xerces()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # target_compile_options(xerces-c PRIVATE -w)
elseif(MSVC)
    target_compile_options(xerces-c PRIVATE /wd4018 /wd4244 /wd4996 )
endif()

add_subdirectory(src/common)
add_subdirectory(src/tpcaccess)

if(BUILD_PYTHON_WRAPPERS)
	add_subdirectory(src/wrappers/pytpcaccess)
endif()

if(BUILD_EXAMPLES)
	add_subdirectory(examples/tpcaccess-examples)
endif()

if(ENABLE_INSTALL)
    function(tpc_install)
        install(${ARGV})
    endfunction()

	include(GNUInstallDirs)
    if(INSTALL_TPACESS)
		install(TARGETS tpcaccess)
    endif()

    if(BUILD_PYTHON_WRAPPERS AND INSTALL_PYTPCACCESS)
        include(cmake/TpcAccessInstallPython.cmake)
    endif()
endif()
