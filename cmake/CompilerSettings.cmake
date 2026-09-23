# CompilerSettings.cmake
# Global compiler flags and settings for the TPC Server project
#
# This file defines common compile options, definitions, and standards
# that are shared across all modules in the project.

include_guard(GLOBAL)

# ============================================================================
# Platform Detection and Naming
# ============================================================================
if(WIN32)
    set(PLATFORM_WINDOWS ON)
    set(PLATFORM_LINUX OFF)
    set(PLATFORM_NAME "windows")
	set(RUNTIME_NAME "msvcrt")
elseif(UNIX)
    set(PLATFORM_LINUX ON)
    set(PLATFORM_WINDOWS OFF)
    set(PLATFORM_NAME "linux")
	set(RUNTIME_NAME "")
endif()

# Architecture detection
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_NAME "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCH_NAME "x86")
else()
    set(ARCH_NAME "unknown")
endif()

# Platform string for directory names (e.g., "linux_x64", "win_x86")
set(PLATFORM_ARCH "${PLATFORM_NAME}_${ARCH_NAME}")

# ============================================================================
# Compiler Detection
# ============================================================================
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMPILER_GCC_CLANG ON)
else()
    set(COMPILER_GCC_CLANG OFF)
endif()

# ============================================================================
# C/C++ Standard
# ============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ============================================================================
# Compiler Setting Defaults
# ============================================================================
add_library(tpc_default_options INTERFACE)

if(MSVC)
    target_compile_options(tpc_default_options INTERFACE
        /MP                              # Multi-processor compilation
        /permissive-                     # Standards conformance
        /Zc:__cplusplus                  # Enable correct __cplusplus macro
        /EHsc                            # Exception handling model

        # Debug configuration
        $<$<CONFIG:Debug>:/Od>           # Disable optimization

        # Release configuration
		$<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Oi>         # Enable intrinsic functions
        $<$<CONFIG:Release>:/Ot>         # Favor fast code
        $<$<CONFIG:Release>:/Z7>         # Generate debug information inside of obj files for LTO
        $<$<CONFIG:Release>:/arch:SSE4.2>
    )

    target_link_options(tpc_default_options INTERFACE
        /DEBUG                           # Generate pdb files in both debug and release builds
        /SUBSYSTEM:CONSOLE               # Set MSVC subsystem to console application

        # Linker options for release configuration
        $<$<CONFIG:Release>:/OPT:REF>    # Eliminate unused functions
        $<$<CONFIG:Release>:/OPT:ICF>    # Identical COMDAT folding
    )
elseif(COMPILER_GCC_CLANG)
    target_compile_options(tpc_default_options INTERFACE
	    -fdiagnostics-color=always       # Colored compiler diagnostic messages

        # Debug configuration
        $<$<CONFIG:Debug>:-O0>           # No optimizations
        #$<$<CONFIG:Debug>:-Og>           # Debug-compatible optimizations
        $<$<CONFIG:Debug>:-g>            # Debug symbols
        $<$<CONFIG:Debug>:-ggdb>         # GDB-specific debug info

        # Release configuration
        $<$<CONFIG:Release>:-O2>         # Aggressive optimization
        $<$<CONFIG:Release>:-DNDEBUG>    # Define NDEBUG
        $<$<CONFIG:Release>:-march=native> # Optimize for host CPU
    )

    # Linker options for release build
    target_link_options(tpc_default_options INTERFACE
        $<$<CONFIG:Release>:-flto=auto>  # Link-time optimization
        $<$<CONFIG:Release>:-s>          # Strip symbols
    )
endif()

if(ENABLE_BUILD_FLAG_DEFAULTS)
    message(STATUS "Using preset build flags")

    # Use static runtime library for MSVC builds
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    link_libraries(
		$<BUILD_LOCAL_INTERFACE:tpc_default_options>
	)
endif()

# ============================================================================
# Compiler Warning Settings
# ============================================================================
# Used by cmake/ProjectHelpers.cmake
add_library(tpc_compiler_warnings INTERFACE)

if(MSVC)
    target_compile_options(tpc_compiler_warnings INTERFACE
        /W4                              # Warning level 4

        # Disable some noisy warnings
        /wd4100                          # Unreferenced formal parameter
        /wd4146                          # Unary minus operator applied to unsigned type
        /wd4245                          # Signed/unsigned mismatch
        /wd5033                          # Register storage class specifier
        /wd4458                          # Declaration hides class member
        #/wd4389                          # Signed/Unsigned Mismatch
    )
elseif(COMPILER_GCC_CLANG)
    target_compile_options(tpc_compiler_warnings INTERFACE
        #-Wall                            # Enable most warnings
        #-Wextra                          # Extra warnings
        #-Wpedantic                       # Pedantic warnings
        -Wno-unused-parameter            # Allow unused parameters
        -Wno-sign-compare                # Allow signed/unsigned comparison
    )
endif()

# ============================================================================
# Whole Program Optimization Flags
# ============================================================================
add_library(tpc_whole_program_optimization INTERFACE)

if(MSVC)
    target_compile_options(tpc_whole_program_optimization INTERFACE
        $<$<CONFIG:Release>:/GL>         # Whole program optimization
    )
    target_link_options(tpc_whole_program_optimization INTERFACE
        $<$<CONFIG:Release>:/LTCG>       # Link-time code generation
    )
elseif(COMPILER_GCC_CLANG)
    target_compile_options(tpc_whole_program_optimization INTERFACE
        $<$<CONFIG:Release>:-flto=auto>
    )
    target_link_options(tpc_whole_program_optimization INTERFACE
        $<$<CONFIG:Release>:-flto=auto>
    )
endif()

if(ENABLE_WHOLE_PROGRAM_OPTIMIZATION)
    message(STATUS "Using whole program optimization flags")
    link_libraries(
		$<BUILD_LOCAL_INTERFACE:tpc_whole_program_optimization>
	)
endif()

# ============================================================================
# Architecture-Specific Settings
# ============================================================================
message(STATUS "Platform: ${PLATFORM_ARCH}")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(ARCH_X64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_compile_definitions(ARCH_X86)
endif()

# ============================================================================
# Global Project Definitions
# ============================================================================
add_compile_definitions(
	$<$<CONFIG:Debug>:_DEBUG>
	$<$<CONFIG:Release>:NDEBUG>
)

# ============================================================================
# Platform-specific Settings
# ============================================================================
if(PLATFORM_WINDOWS)
	add_compile_definitions(
		WIN32
		_WINDOWS
		UNICODE
		_UNICODE
	)
elseif(PLATFORM_LINUX)
	add_compile_definitions(
		_LINUX
		TRANET_DEVICE
	)

    # Threading support
    set(CMAKE_THREAD_PREFER_PTHREAD ON)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

# ============================================================================
# Global Target Properties
# ============================================================================
# Position-independent code for all targets (required for static libs linked into shared libs/executables)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export Compile Commands JSON")

# ============================================================================
# musl Compilation Options
# ============================================================================
# Compile options for generating fully statically linked libraries and executables
add_library(tpc_musl_options INTERFACE)

target_compile_options(tpc_musl_options INTERFACE
	-fPIC
)

target_link_options(tpc_musl_options INTERFACE
    -static-libgcc
    -static-libstdc++
    -fPIC

    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:-Wl,--exclude-libs,ALL>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:-nodefaultlibs>
)

target_link_libraries(tpc_musl_options INTERFACE
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:-Wl,--start-group>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${CMAKE_SYSROOT}/lib/libatomic.a>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${CMAKE_SYSROOT}/lib/libstdc++.a>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:-lgcc>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:-lgcc_eh>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${CMAKE_SYSROOT}/lib/libc.a>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:-Wl,--end-group>
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	target_compile_options(tpc_musl_options INTERFACE
		-march=x86-64-v2 # Support up to SSE4.2, but no AVX
		-mtune=tremont   # Optimize for TraNET cpus
	)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	target_compile_options(tpc_musl_options INTERFACE
		-march=silvermont # Optimize for our atom cpus
	)
endif()

# TODO refactor this to also contain MSVC static compile options
add_library(tpc_static_executable INTERFACE)

target_compile_options(tpc_static_executable INTERFACE
	-fPIE
)

target_link_options(tpc_static_executable INTERFACE
	-fPIE
	-static
)

if(USING_MUSL_TOOLCHAIN AND NOT DISABLE_MUSL_DEFAULTS)
    link_libraries(
		$<BUILD_LOCAL_INTERFACE:tpc_musl_options>
		$<BUILD_LOCAL_INTERFACE:$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:tpc_static_executable>>
	)

    message(STATUS "Disabling sanitizers due to musl toolchain incompatibility")
	set(DISABLE_SANITIZERS ON CACHE BOOL "Disable sanitizers in all build configurations" FORCE) 
endif()

# ============================================================================
# Sanitizer Compilation Options
# ============================================================================
add_library(tpc_sanitizers INTERFACE)
add_library(tpc_no_sanitizers INTERFACE)

if(MSVC)
    if(ENABLE_THREAD_SANITIZER)
        message(FATAL_ERROR "Thread Sanitizer is not supported on MSVC")
    else()
		target_compile_options(tpc_sanitizers INTERFACE /fsanitize=address) 
		target_link_options(tpc_sanitizers INTERFACE /INCREMENTAL:NO) 
        set(SANITIZERS_USED "(asan)")
    endif()

    # Options which we would like when sanitizers are disabled
	target_compile_options(tpc_no_sanitizers INTERFACE 
        $<$<CONFIG:Debug>:/RTC1>       # Runtime checks
        $<$<CONFIG:Debug>:/ZI>         # Hot Compilation Debug-info
    ) 
else()
	# Thread Sanitizer (incompatible with ASAN and LSAN)
    if(ENABLE_THREAD_SANITIZER)
		# Enabling ThreadSanitizer might lead to the following error on some Linux systems:
		# FATAL: ThreadSanitizer: unexpected memory mapping 0x5c9bd4d2b000-0x5c9bd4d4b000
		# Fix with "sudo sysctl vm.mmap_rnd_bits=28"
		target_compile_options(tpc_sanitizers INTERFACE -fsanitize=thread)
		target_link_options(tpc_sanitizers INTERFACE -fsanitize=thread)
        target_compile_definitions(tpc_sanitizers INTERFACE TSAN_ENABLED)
        set(SANITIZERS_USED "(tsan)")
    else()
		target_compile_options(tpc_sanitizers INTERFACE
			-fsanitize=address
			-fsanitize=leak
		)
		target_link_options(tpc_sanitizers INTERFACE
			-fsanitize=address
			-fsanitize=leak
		)
        set(SANITIZERS_USED "(asan, lsan)")
    endif()
endif()

if(FORCE_ENABLE_SANITIZERS)
    link_libraries($<BUILD_LOCAL_INTERFACE:tpc_sanitizers>) 
    set(SANITIZER_STATUS "ON, all configs")
elseif(ENABLE_SANITIZER_DEFAULTS AND NOT DISABLE_SANITIZERS)
    link_libraries(
        $<$<CONFIG:Debug>:$<BUILD_LOCAL_INTERFACE:tpc_sanitizers>>
		$<$<CONFIG:Release>:$<BUILD_LOCAL_INTERFACE:tpc_no_sanitizers>>
    ) 
    set(SANITIZER_STATUS "ON, debug configs")
else()
    link_libraries($<BUILD_LOCAL_INTERFACE:tpc_no_sanitizers>) 
    set(SANITIZERS_USED "")
    set(SANITIZER_STATUS "OFF")
endif()

# ============================================================================
# C Runtime Detection
# ============================================================================

# get_runtime_info()
#
# Returns a string describing the C runtime configuration
# - Windows: Static/dynamic and debug/release runtime (e.g., "MultiThreadedDebug")
# - Linux: libc type and linking mode (e.g., "musl, static linking")
#
# Usage:
#   get_runtime_info(RUNTIME_INFO)
#   message(STATUS "C runtime: ${RUNTIME_INFO}")
#
function(get_runtime_info OUT_VAR)
    set(RESULT "Unknown")

    if(MSVC)
        if(CMAKE_MSVC_RUNTIME_LIBRARY)
            set(RESULT "winrt, static")
        endif()
    else()
        if(USING_MUSL_TOOLCHAIN)
            set(RESULT "musl, static")
        else()
            set(RESULT "glibc, dynamic")
        endif()
    endif()

    set(${OUT_VAR} "${RESULT}" PARENT_SCOPE)
endfunction()
