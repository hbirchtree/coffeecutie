find_package ( Microsoft.GSL REQUIRED )
find_package ( tinyutf8 CONFIG REQUIRED )

coffee_library (
    TARGET PeripheralLib
    SOURCES
    private/error_types.cpp
    private/build_info.cpp
    private/libc_functions.cpp
    private/pimpl_state.cpp

    LIBRARIES Microsoft.GSL::GSL tinyutf8::tinyutf8

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

if(BUILD_ASAN)
    target_compile_options ( PeripheralLib PUBLIC
        -fsanitize=address
        )
    target_link_options ( PeripheralLib PUBLIC
        -fsanitize=address
        )
endif()

if(EMSCRIPTEN)
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options ( PeripheralLib PUBLIC
            -gsource-map
            )
        target_link_options ( PeripheralLib PUBLIC
            -gsource-map
            -sDEMANGLE_SUPPORT=1
            -sASSERTIONS=1
            )
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        target_compile_options ( PeripheralLib PUBLIC
            -gsource-map -O1
#            -sDEMANGLE_SUPPORT=1
            )
        target_link_options ( PeripheralLib PUBLIC
            -gsource-map -O1
            -sDEMANGLE_SUPPORT=1
            -sASSERTIONS=1
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

if(MINGW)
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
        mingw32
        # Requires Windows 8+
        cabinet
        )
    target_compile_definitions ( PeripheralLib PUBLIC
        -DCOFFEE_WINDOWS_COMPRESSION_AVAILABLE=1
        )
elseif(WIN_UWP)
    target_link_libraries ( PeripheralLib PUBLIC
        OneCore
        cabinet
        )
    target_compile_definitions ( PeripheralLib PUBLIC
        -DCOFFEE_WINDOWS_COMPRESSION_AVAILABLE=1
        )
elseif(WIN32)
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
        # Requires Windows 8+
        cabinet
        )
    target_compile_definitions ( PeripheralLib PUBLIC
        -DCOFFEE_WINDOWS_COMPRESSION_AVAILABLE=1
        )
elseif(ANDROID)
    target_link_libraries ( PeripheralLib PUBLIC
        # For logcat logging
        log
        # Most Android-specifics are in here
        android
        )
elseif(APPLE)
    if(NOT IOS)
        target_link_libraries ( PeripheralLib PUBLIC
            "-framework CoreGraphics"
            "-framework QuartzCore"
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
        m
        )
elseif(EMSCRIPTEN)
    target_compile_options ( PeripheralLib PUBLIC
        -fwasm-exceptions
        -fPIC
        -pthread
        -flto
       )
    target_link_options ( PeripheralLib PUBLIC
        -fwasm-exceptions
        -fPIC
        -flto
        -pthread
        --no-heap-copy
        -sLLD_REPORT_UNDEFINED
#        -sSAFE_HEAP=1
#        -sALLOW_MEMORY_GROWTH
       )
    if("${CMAKE_BUILD_TYPE}" MATCHES "Deb")
        target_link_options( PeripheralLib PUBLIC
            )
    endif()
#    if("${CMAKE_BUILD_TYPE}" MATCHES "Rel")
#        target_compile_options ( PeripheralLib PUBLIC
#            )
#        target_link_options ( PeripheralLib PUBLIC
#            )
#    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_package ( Threads REQUIRED )
    target_link_libraries ( PeripheralLib PUBLIC
        rt
        dl
        m
        Threads::Threads
        stdc++fs
        )
elseif(GAMECUBE)
    target_link_libraries ( PeripheralLib PUBLIC
        ogc
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

add_subdirectory ( format )
add_subdirectory ( platform )

add_subdirectory ( compressors/zlib )
add_subdirectory ( compressors/zstd )
if(WIN32)
    add_subdirectory ( compressors/cab )
endif()
