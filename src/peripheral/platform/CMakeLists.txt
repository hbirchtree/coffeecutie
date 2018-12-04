coffee_library (
    TARGET PeripheralPlatform
    LIBRARIES
    PeripheralLib

    SOURCES
    private/argument_parse.cpp
    private/platform_data.cpp
    private/shared_func.cpp
    private/url.cpp

    #
    # This one is pretty important, let's keep it here
    #
    private/profiling.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

#
#
# SDL power management info
#
#
if(BUILD_SDL2 AND NOT POWERPC AND NOT IOS AND
        (ANDROID OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR WIN32))

    if(NOT TARGET SDL2)
        find_package ( SDL2 QUIET )
    endif()

    coffee_library (
        TARGET CoffeeSDLPowerInfo
        SOURCES private/sdlpowerinfo.cpp
        LIBRARIES PeripheralLib SDL2
        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
endif()

#
#
# Android, there's a lot of weird stuff here because NativeActivity sucks
# Generally should not be touched
#
#
if(ANDROID)
    find_package( AndroidGlue REQUIRED )

#    target_sources ( CoffeeCore_Application PRIVATE
#        platform/android/and_main.cpp
#        ${ANDROID_GLUE_SOURCES}
#        )

    target_include_directories ( PeripheralPlatform PRIVATE
        platform/android/include
        )

    target_sources ( PeripheralPlatform PRIVATE
        ${NDK_HELPER_SOURCES}
        ${CPUFEATURES_SOURCES}

        platform/android/JNIHelper.cpp
        platforms/android/environment.cpp
        platforms/android/sensor.cpp
        platforms/android/file.cpp
        platforms/android/sysinfo.cpp
        platforms/linux/file.cpp
        platforms/linux/process.cpp
        platforms/linux/sysinfo.cpp
        )

    target_compile_definitions ( PeripheralPlatform PUBLIC
        -DANDROID_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
        )

    target_compile_options ( PeripheralPlatform PUBLIC
        # This flag gives a lot of false positives for aligned accesses
        -Wno-address-of-packed-member
        )

    target_include_directories ( PeripheralPlatform PUBLIC
        $<BUILD_INTERFACE:${ANDROID_GLUE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NDK_HELPER_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CPUFEATURES_INCLUDE_DIR}>
        )
endif()

#
#
# Apple iOS/OS X-specific code
#
#
if(APPLE)
    if(NOT IOS)
        target_link_libraries ( PeripheralPlatform PUBLIC
            "-framework CoreGraphics"
            "-framework QuartzCore"
            "-framework OpenGL"
            "-framework AppKit"
            )
    endif()

    target_sources ( PeripheralPlatform PRIVATE
        #
        # System information queries
        #
        platforms/osx/sysinfo.cpp
        platforms/osx/environment.cpp
        # Some functions are exposed by Obj-C
        platforms/osx/system_interface.m

        platforms/osx/file.cpp
        )
endif()

#
#
# Portable POSIX code
#
#
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR ANDROID)
    target_sources ( PeripheralPlatform PRIVATE

        #
        # POSIX environment functions
        #
        platforms/posix/environment.cpp

        #
        # Stacktrace using libunwind
        #
        platforms/posix/stacktrace.cpp

        #
        # Dynamic linking (libdl)
        #
        platforms/posix/loader.cpp

        #
        # File management
        #
        platforms/posix/file.cpp
        )

    find_package( Unwind QUIET )
    if(UNWIND_FOUND)
        add_definitions(
            -DLIBUNWIND_ENABLED
            )
    endif()
endif()

#
#
# Linux desktop-specific code
#
#
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    target_sources ( PeripheralPlatform PRIVATE
        #
        # Linux-specific system calls and behavior all around
        #
        platforms/linux/environment.cpp
        platforms/linux/stacktrace.cpp
        platforms/linux/process.cpp
        platforms/linux/sysinfo.cpp

        platforms/linux/file.cpp
        )
endif()

#
#
# Emscripten-specific code
#
#
if(EMSCRIPTEN)
    target_sources ( PeripheralPlatform PRIVATE
        #
        # Like POSIX, but limited
        #
        platforms/posix/environment.cpp
        platforms/emscripten/sysinfo.cpp
        )
endif()

#
#
# Raspberry Pi-specific code
#
#
if(RASPBERRY)
    target_sources ( PeripheralPlatform PRIVATE
        #
        # Extracts Raspberry Pi board name
        #
        devices/raspberry/sysinfo.cpp
        )
endif()

#
#
# Maemo-specific code
#
#
if(MAEMO)
    target_sources ( PeripheralPlatform PRIVATE
        #
        # Accelerometers, light sensors and etc.
        #
        devices/maemo/sensors.cpp
        )
endif()

#
#
# Windows Win32/UWP-specific code
#
#
if(WIN32)
    target_sources ( PeripheralPlatform PRIVATE
        platforms/win32/environment.cpp
        platforms/win32/sysinfo.cpp

        platforms/win32/file.cpp

#        platforms/win32/errors.cpp
        )
    if(NOT WIN_UWP AND NOT MINGW64)
        target_sources ( PeripheralPlatform PRIVATE
            platforms/win32/windows_startup.cpp
            )
    elseif(WIN_UWP)
        target_sources ( PeripheralPlatform PRIVATE
            platforms/win32/windows_startup_uwp.cpp
            )
    endif()
endif()
