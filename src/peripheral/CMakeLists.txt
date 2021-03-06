coffee_library (
    TARGET PeripheralLib
    SOURCES
    private/error_types.cpp
    private/build_info.cpp
    private/libc_functions.cpp
    private/pimpl_state.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

gen_version_header ( PeripheralLib )

target_compile_definitions ( PeripheralLib PRIVATE
    -DCOFFEE_BUILD_STRING=${COFFEE_BUILD_STRING}
    )

target_precompile_headers ( PeripheralLib PUBLIC
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/libc/types.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/functional_types.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/math.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/string_casting.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/string_ops.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/thread_types.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/threads/job_system.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/types.h>
#    $<$<COMPILE_LANGUAGE:CXX>:peripherals/stl/type_list.h>
    )

if(EMSCRIPTEN)
    if("${EMSCRIPTEN_SOURCEMAP_BASE}" STREQUAL "")
        set ( EMSCRIPTEN_SOURCEMAP_BASE "http://localhost:8000" )
    endif()

    set ( EMSCRIPTEN_SOURCEMAP_BASE "${EMSCRIPTEN_SOURCEMAP_BASE}"
        CACHE STRING "Emscripten sourcemap base URL"
        )

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options ( PeripheralLib PUBLIC
            -g4
            "-s                              " DEMANGLE_SUPPORT=1
            )
        target_link_options ( PeripheralLib PUBLIC
            -g4
            "-s                            " DEMANGLE_SUPPORT=1
            --source-map-base "${EMSCRIPTEN_SOURCEMAP_BASE}"
            )
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        target_compile_options ( PeripheralLib PUBLIC
            -g4 -O1
            "-s                                 " DEMANGLE_SUPPORT=1
            )
        target_link_options ( PeripheralLib PUBLIC
            -g4 -O1
            "-s                                " DEMANGLE_SUPPORT=1
            --source-map-base "${EMSCRIPTEN_SOURCEMAP_BASE}"
            )
    else()
        target_compile_options ( PeripheralLib PUBLIC
            -O3
            )
        target_link_options ( PeripheralLib PUBLIC
            -O3
            )
    endif()
endif()

###########################################################
# Base system libraries
###########################################################

if(WIN32)
    if(WIN_UWP)
        target_link_libraries ( PeripheralLib PUBLIC
            OneCore
            )
    else()
        target_link_libraries ( PeripheralLib PUBLIC
            user32
            gdi32
            winmm
            imm32
            ole32
            oleaut32
            version
            uuid
            dinput8
            shell32
            ws2_32
            wbemuuid
            netapi32
            )
    endif()

    if(MINGW)
        target_link_libraries ( PeripheralLib PUBLIC
            mingw32
            )
    endif()

    target_link_libraries ( PeripheralLib PUBLIC
        # For compression using Windows built-in zlib-like
        # This is only available on Windows 8+
        cabinet
        )
    target_compile_definitions ( PeripheralLib PUBLIC
        -DCOFFEE_WINDOWS_COMPRESSION_AVAILABLE=1
        )
endif()

if(ANDROID)
    target_link_libraries ( PeripheralLib PUBLIC
        # For logcat logging
        log
        # Most Android-specifics are in here
        android
        )
endif()

if(APPLE)
    if(NOT IOS)
        target_link_libraries ( PeripheralLib PUBLIC
            "-framework CoreGraphics"
            "-framework QuartzCore"
            "-framework OpenGL"
            "-framework AppKit"
            )
    else()
        target_link_libraries ( PeripheralLib PUBLIC
            "-framework CoreGraphics"
            "-framework CoreMotion"
            "-framework QuartzCore"
            "-framework UIKit"
            )
    endif()

    target_link_libraries (PeripheralLib PUBLIC
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework Foundation"
        )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    # Libunwind is used to print function names at runtime
    # Windows does not support this library
    # Android's Bionic does not grant access to unwind, even though it is there. Bad Google, bad!
    if(NOT DEFINED LIBUNWIND_INCLUDE_DIR)
        find_package(Unwind QUIET )
    endif()
    if (LIBUNWIND_FOUND)
        target_include_directories ( PeripheralLib PUBLIC
            $<BUILD_INTERFACE:${LIBUNWIND_INCLUDE_DIR}>
            )
        target_link_libraries ( PeripheralLib PUBLIC 
            $<BUILD_INTERFACE:${LIBUNWIND_LIBRARIES}>
            )
    endif()

    target_link_libraries ( PeripheralLib PUBLIC
        rt
        )
endif()

if(NOT WIN32 AND NOT MSYS AND NOT MINGW AND NOT EMSCRIPTEN)
    if(NOT APPLE AND NOT GAMECUBE)
        target_link_libraries ( PeripheralLib PUBLIC
            dl
            )
    endif()
    target_link_libraries ( PeripheralLib PUBLIC
        m
        )
endif()

if(NOT WIN32 AND NOT APPLE AND NOT EMSCRIPTEN AND NOT GAMECUBE)
    find_package ( Threads REQUIRED )
    target_link_libraries ( PeripheralLib PUBLIC
        Threads::Threads
        )
endif()

if(GAMECUBE)
    target_link_libraries ( PeripheralLib PUBLIC
        ogc
        )
endif()

if(EMSCRIPTEN)
    target_compile_options ( PeripheralLib PUBLIC
        "-s            " ALLOW_MEMORY_GROWTH=1
        "-s           " SAFE_HEAP=1
        "-s          " DISABLE_EXCEPTION_CATCHING=0
        "-s         " DEMANGLE_SUPPORT=1
        "--no-heap-copy"
        "-fPIC"
       )
   target_link_options ( PeripheralLib PUBLIC
        "-s            " ALLOW_MEMORY_GROWTH=1
        "-s           " SAFE_HEAP=1
        "-s          " DISABLE_EXCEPTION_CATCHING=0
        "-s         " DEMANGLE_SUPPORT=1
        "--no-heap-copy"
        "-fPIC"
       )
endif()

############################################################
# STL extensions, replacing STL where it does not exist
############################################################


############################################################
# Backup thread naming
#  - for platforms where thread naming doesn't really exist
############################################################

coffee_library (
    TARGET PeripheralThreadNaming
    LIBRARIES PeripheralPlatform
    SOURCES
    stl_extensions/thread_naming.cpp
    )

#
#
# Alternative thread implementations
# Used for platforms without std::thread/std::mutex support
# Implemented here:
#  - Gamecube/Wii LWP
#
#
if(GAMECUBE)
    coffee_library (
        TARGET GeckoThreads
        LIBRARIES PeripheralLib
        SOURCES
        stl_replacements/gekko/threads.cpp
        )
    target_link_libraries ( PeripheralThreadNaming INTERFACE
        GeckoThreads
        )
endif()

############################################################
# Platform layer
############################################################

add_subdirectory ( platform )
