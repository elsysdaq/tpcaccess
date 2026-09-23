# CMakeLists.txt for common library
# Static library providing shared utilities and cross-cutting concerns

set(TARGET common)

generate_version_header("${CMAKE_CURRENT_SOURCE_DIR}/version.h" "common")

# Create the library target
add_library(${TARGET} STATIC)
add_library(tpcserver::${TARGET} ALIAS ${TARGET})

# Use the helper function with filtered sources
add_tpc_project(common)

target_include_directories(${TARGET}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/gsoap
)

# Module-specific compile definitions
target_compile_definitions(${TARGET}
    PUBLIC
        SOAP_BUFLEN=1048576
)

# Module-specific compiler options
if(MSVC)
    target_compile_options(${TARGET}
        PRIVATE
            /wd4310  # Cast truncates constant value
            /wd4211  # Redefined extern to static
            /wd5033  # Register storage class specifier
    )
endif()

if(COMPILER_GCC_CLANG)
    target_compile_options(${TARGET}
        PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>
    )

	# Module-specific compile definitions
	target_compile_definitions(${TARGET}
		PUBLIC
			__GLIBC__
		    SOCKLEN_T=socklen_t
	)
endif()

# Threading support (required by sysThreading.cpp)
if(PLATFORM_LINUX)
    target_link_libraries(${TARGET}
        PUBLIC
            Threads::Threads
    )
endif()

# Windows-specific libraries
if(PLATFORM_WINDOWS)
    target_link_libraries(${TARGET}
        PUBLIC
            ws2_32  # Winsock (for gSOAP)
    )
endif()
