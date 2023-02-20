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

if((${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE))
    find_library ( BACKTRACE_LIBRARY backtrace )
    if (BACKTRACE_LIBRARY AND NOT EMSCRIPTEN AND NOT ANDROID)
        target_link_libraries ( PeripheralPlatform PUBLIC
            ${BACKTRACE_LIBRARY}
            )
    else()
        target_compile_definitions ( PeripheralPlatform PUBLIC
            -DCOFFEE_NO_LIB_BACKTRACE
            )
    endif()
else()
    target_compile_definitions ( PeripheralPlatform PUBLIC
        -DCOFFEE_NO_LIB_BACKTRACE
        )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options( PeripheralPlatform PUBLIC
        -Werror
        -Wall
#        -Wpedantic
        -Wextra
        -Winit-self
        -Wreturn-type
        -Wpointer-arith
        -Wexpansion-to-defined
        # Misfires on code that is ifdef'd
        -Wno-misleading-indentation
        )
endif()

#
#
# SDL power management info
#
#
if(BUILD_SDL2 AND NOT POWERPC AND NOT IOS AND
        (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR WIN32) AND NOT EMSCRIPTEN)

    if(NOT TARGET SDL2)
        find_package ( SDL2 CONFIG QUIET )
    endif()

    coffee_library (
        TARGET SDLPowerInfo
        SOURCES private/sdlpowerinfo.cpp
        LIBRARIES PeripheralLib SDL2::SDL2-static
        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

    dependency_link (
        TARGET SDLPowerInfo
        DEPENDENCIES
            sdl2
        )
endif()

#
#
# Android, there's a lot of weird stuff here because NativeActivity sucks
# Generally should not be touched
#
#
if(ANDROID)
    set ( ANDROID_SOURCES_DIR "${ANDROID_NDK}/sources/android" )

    target_sources ( PeripheralPlatform PRIVATE
#        platforms/android/sensor.cpp
#        platforms/android/file.cpp
        platforms/android/sysinfo.cpp
#        platforms/linux/file.cpp
#        platforms/linux/process.cpp
#        platforms/linux/sysinfo.cpp

        platforms/android/jnipp.cpp
        platforms/android/JNIHelper.cpp
        )

    target_compile_definitions ( PeripheralPlatform PUBLIC
        -DANDROID_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
        )

    target_compile_options ( PeripheralPlatform PUBLIC
        # This flag gives a lot of false positives for aligned accesses
        -Wno-address-of-packed-member
        )

    target_include_directories ( PeripheralPlatform PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platforms/android/include>
        $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/native_app_glue>
        $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/ndk_helper>
        $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/cpufeatures>
        )
    header_install (
        PeripheralPlatform
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platforms/android/include/coffee/jni>
        coffee
        )
endif()

#
#
# Apple iOS/OS X-specific code
#
#
if(APPLE)
    target_sources ( PeripheralPlatform PRIVATE
        #
        # System information queries
        #
        platforms/osx/sysinfo.mm
        # Some functions are exposed by Obj-C
        platforms/osx/system_interface.mm
        )
    if (IOS)
        target_link_libraries ( PeripheralPlatform PRIVATE
            "-framework UIKit"
            )
    else()
        target_link_libraries ( PeripheralPlatform PRIVATE
            "-framework AppKit"
            "-framework Foundation"
            )
    endif()
endif()

#
#
# Portable POSIX code
#
#
if((${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR ANDROID))
#    target_sources ( PeripheralPlatform PRIVATE

        #
        # POSIX environment functions
        #
#        platforms/posix/environment.cpp

        #
        # Stacktrace using libunwind
        #
#        platforms/posix/stacktrace.cpp

        #
        # File management
        #
#        platforms/posix/file.cpp
#        )

    if(NOT IOS)
#        target_sources ( PeripheralPlatform PRIVATE
            #
            # Dynamic linking (libdl)
            #
#            platforms/posix/loader.cpp
#            )
    endif()

    if(APPLE)
        target_compile_definitions( PeripheralPlatform PUBLIC
            -D_GNU_SOURCE)
    endif()

    find_package( Unwind QUIET )
    if(LIBUNWIND_FOUND AND NOT BEAGLE)
        target_compile_definitions( PeripheralPlatform PUBLIC
            -DLIBUNWIND_ENABLED=1
            -DCOFFEE_UNWIND_STACKTRACE=1
            )
        target_link_libraries ( PeripheralPlatform PUBLIC
            $<BUILD_INTERFACE:${LIBUNWIND_LIBRARIES}>
            )
        target_include_directories ( PeripheralPlatform PUBLIC
            $<BUILD_INTERFACE:${LIBUNWIND_INCLUDE_DIR}>
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
#        platforms/linux/environment.cpp
#        platforms/linux/process.cpp
#        platforms/linux/sysinfo.cpp

#        platforms/linux/file.cpp
        )

    coffee_library (
        TARGET EmbedFileStub
        LIBRARIES PeripheralLib
        SOURCES private/embed_files_stub.cpp

        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

    if(EMBED_RESOURCES)
        target_compile_definitions ( PeripheralPlatform PUBLIC
            -DEMBED_RESOURCES_ENABLED=1
            )
    endif()
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
#        platforms/posix/environment.cpp
#        platforms/posix/file.cpp
        platforms/emscripten/sysinfo.cpp
        )
endif()

#
#
# Raspberry Pi-specific code
#
#
#if(RASPBERRY)
#    target_sources ( PeripheralPlatform PRIVATE
#        #
#        # Extracts Raspberry Pi board name
#        #
##        devices/raspberry/sysinfo.cpp
#        )
#endif()

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
#        platforms/win32/environment.cpp
        platforms/win32/sysinfo.cpp

#        platforms/win32/file.cpp
        )
    if(WIN_UWP)
        target_sources ( PeripheralPlatform PRIVATE
            platforms/win32/wmi_uwp.cpp
            )
    else()
        target_sources ( PeripheralPlatform PRIVATE
            platforms/win32/wmi_win32.cpp
            )
    endif()
endif()

#
#
# Gamecube/Wii-specific code
#
#
if(GAMECUBE OR WII)
    target_sources ( PeripheralPlatform PRIVATE
#        platforms/posix/environment.cpp
#        platforms/posix/file.cpp
        platforms/posix/stacktrace.cpp
        )
endif()
