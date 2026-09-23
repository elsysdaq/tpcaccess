# Minimal CMake target for prebuilt tpcaccess library

cmake_minimum_required(VERSION 3.25)

# Create imported target
add_library(tpcaccess-prebuilt SHARED IMPORTED GLOBAL)
add_library(tpcserver-examples::tpcaccess-prebuilt ALIAS tpcaccess-prebuilt)


if(NOT PLATFORM_WINDOWS)
	message(FATAL_ERROR "Prebuilt shared library currently is only available for windows builds")
endif()

# Set include directories (platform-specific due to generated headers)
set_target_properties(tpcaccess-prebuilt PROPERTIES
	IMPORTED_LOCATION             "${CMAKE_CURRENT_LIST_DIR}/bin/x86_64-windows-release/TpcAccess.dll"
    IMPORTED_IMPLIB               "${CMAKE_CURRENT_LIST_DIR}/bin/x86_64-windows-release/TpcAccess.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../../include"
)
