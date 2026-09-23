set(TARGET pytpcaccess)
find_package(SWIG 4.0 REQUIRED COMPONENTS python)
include(UseSWIG)

if(DEFINED ${SKBUILD_SABI_COMPONENT})
	find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module ${SKBUILD_SABI_COMPONENT})
else()
	find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module 
	                     OPTIONAL_COMPONENTS Development.SABIModule)
endif()

set(TPCACCESS_SWIG_INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/tpcaccess.i")
set(TPCACCESS_SWIG_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")

set_source_files_properties(${TPCACCESS_SWIG_INTERFACE}
	PROPERTIES
		CPLUSPLUS ON
		USE_SWIG_DEPENDENCIES ON
		USE_TARGET_INCLUDE_DIRECTORIES ON
		DEPENDS "${PROJECT_SOURCE_DIR}/include/TpcAccess.h;${PROJECT_SOURCE_DIR}/src/tpcaccess/TpcAccessA.h"
)

swig_add_library(${TARGET}
	TYPE MODULE
	LANGUAGE python
	OUTPUT_DIR "${TPCACCESS_SWIG_GENERATED_DIR}"
	OUTFILE_DIR "${TPCACCESS_SWIG_GENERATED_DIR}"
	SOURCES ${TPCACCESS_SWIG_INTERFACE}
)

target_compile_definitions(${TARGET} PRIVATE
    Py_LIMITED_API=0x03090000
)

# Keep the names expected by the generated proxy: TpcAccess.py imports
# the native _TpcAccess extension.
set_target_properties(${TARGET}
	PROPERTIES
		OUTPUT_NAME TpcAccess
		ARCHIVE_OUTPUT_NAME ${TARGET}
		FOLDER src/wrappers
)

target_include_directories(${TARGET}
	PRIVATE
		"${PROJECT_SOURCE_DIR}/include"
		"${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${TARGET}
	PRIVATE
		tpcserver::tpcaccess_static
		Python3::SABIModule
)

# Most Windows Python distributions do not ship a debug interpreter or
# pythonXY_d.lib. SWIG supports compiling a Debug extension against the
# release Python ABI when CMake had to select the release import library.
if(MSVC AND NOT Python3_LIBRARY_DEBUG)
	target_compile_definitions(${TARGET}
		PRIVATE
			$<$<CONFIG:Debug>:SWIG_PYTHON_INTERPRETER_NO_DEBUG>
	)
endif()

# Multi-config generators put the extension in a configuration-specific
# directory. Copy the proxy beside it so that the directory is directly
# importable with PYTHONPATH.
add_custom_command(TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${TPCACCESS_SWIG_GENERATED_DIR}/TpcAccess.py"
		"$<TARGET_FILE_DIR:${TARGET}>/TpcAccess.py"
	VERBATIM
)

message(STATUS "Adding target ${TARGET} (${CMAKE_CURRENT_SOURCE_DIR})")
