# CMakeLists.txt for tpcaccess
# Client access library for TPC Server

generate_version_header("${CMAKE_CURRENT_SOURCE_DIR}/version.h" "tpcaccess")

include(${PROJECT_SOURCE_DIR}/cmake/PrebuildLibs.cmake)

# Builds both shared and static libraries. To avoid duplicating compilation we have a third object libray target

################################################################################
# Object Library
################################################################################

set(TPCACCESS_HEADERS ${PROJECT_SOURCE_DIR}/include/TpcAccess.h TpcAccessA.h TpcAccessElsys.h)
set(TARGET-OB tpcaccess_object)

# Common sources for both platforms
set(TPCACCESS_SOURCES
    AssocRow.cpp
    Attributes.cpp
    DataReceiver.cpp
    Device.cpp
    DeviceInfo.cpp
    ParameterSet.cpp
    SoapDevice.cpp
    StringConversions.cpp
    System.cpp
    SystemList.cpp
    TpcSettings.cpp
    TpcSettingsFromXml.cpp
    TpcSettingsToXml.cpp
    XercesUtils.cpp
    xerces_link.cpp
    YMetaData.cpp
    ${TPCACCESS_HEADERS}
)

# Windows-only sources
if(PLATFORM_WINDOWS)
    list(APPEND TPCACCESS_SOURCES
        ResourceFile.cpp
        TpcAccessInstance.cpp
    )
endif()

add_library(${TARGET-OB} OBJECT)
add_library(tpcserver::${TARGET-OB} ALIAS ${TARGET-OB})

# Use helper function to add headers and set up basic configuration
add_tpc_project(${TARGET-OB} SOURCES ${TPCACCESS_SOURCES})

# Module-specific compile definitions
target_compile_definitions(${TARGET-OB}
    PRIVATE
        XML_LIBRARY
)

# Module-specific compiler options
if(MSVC)
    target_compile_options(${TARGET-OB}
        PRIVATE
            /wd4996  # Deprecated function
            /wd5054  # Operator '==': deprecated between enumerations of different types 
    )
endif()

set_target_properties(${TARGET-OB}
    PROPERTIES
        FOLDER src/tpcaccess
)

target_link_libraries(${TARGET-OB}
    PUBLIC
        tpcserver::common
        xerces-c
        $<$<PLATFORM_ID:Windows>:wsock32;ws2_32>
)

################################################################################
# Shared Library
################################################################################

set(TARGET-SH tpcaccess)

set(TPCACCESS_SHARED_LIB_SOURCES
    main.cpp
    TpcAccess.cpp
    ${TPCACCESS_HEADERS}
)

# Windows-only sources
if(PLATFORM_WINDOWS)
    list(APPEND TPCACCESS_SHARED_LIB_SOURCES
		tpcaccess.rc
    )
endif()

add_library(${TARGET-SH} SHARED ${TPCACCESS_SHARED_LIB_SOURCES})
add_library(tpcserver::${TARGET-SH} ALIAS ${TARGET-SH})
add_tpc_project_properties(${TARGET-SH})

if(MSVC)
    target_compile_definitions(${TARGET-SH}
		PRIVATE
			TPC_EXP=__declspec\(dllexport\)  # Required to export symbols when building .dll
		INTERFACE
			TPC_EXP=__declspec\(dllimport\)  # For consumers of shared library
    )
endif()

# Musl build makes symbols hidden by default, with this attribute we selectively export the API symbols again
if(USING_MUSL_TOOLCHAIN)
    target_compile_definitions(${TARGET-SH}
		PRIVATE
            TPC_EXP=__attribute__\(\(visibility\(\"default\"\)\)\)
    )
endif()

get_version_metadata("${PROJECT_SOURCE_DIR}/build/version_metadata.json" PROJECT "tpcaccess")
# Set output name
set_target_properties(${TARGET-SH}
    PROPERTIES
        OUTPUT_NAME "TpcAccess"
        VERSION "${TPCACCESS_VERSION_STR}"
        SOVERSION "${TPCACCESS_MAJOR}"
        FOLDER src/tpcaccess
)

target_link_libraries(${TARGET-SH}
    PUBLIC
        xerces-c
        tpcserver::common
        tpcserver::tpcaccess_object
        $<$<PLATFORM_ID:Windows>:wsock32;ws2_32>
)

add_prebuild_library(${TARGET-SH}
	LIBRARY_DIR examples/tpcaccess-prebuilt/bin/${PRESET_FULL_NAME}
)

################################################################################
# Static Library
################################################################################

set(TARGET-ST tpcaccess_static)

set(TPCACCESS_STATIC_LIB_SOURCES
    main.cpp
    TpcAccess.cpp
    ${TPCACCESS_HEADERS}
)

add_library(${TARGET-ST} STATIC ${TPCACCESS_STATIC_LIB_SOURCES})
add_library(tpcserver::${TARGET-ST} ALIAS ${TARGET-ST})
add_tpc_project_properties(${TARGET-ST})

# Set output name
set_target_properties(${TARGET-ST}
    PROPERTIES
        # OUTPUT_NAME "TpcAccess"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        FOLDER src/tpcaccess
)

target_link_libraries(${TARGET-ST}
    PUBLIC
        xerces-c
        tpcserver::common
        tpcserver::tpcaccess_object
        $<$<PLATFORM_ID:Windows>:wsock32;ws2_32>
)
