option ( BUILD_WITH_WERROR ON "Build with warnings as errors" )

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

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND
   BUILD_WITH_WERROR)
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
# Android, there's a lot of weird stuff here because NativeActivity sucks
# Generally should not be touched
#
#
if(ANDROID)
    set ( ANDROID_SOURCES_DIR "${ANDROID_NDK}/sources/android" )

    target_sources ( PeripheralPlatform PRIVATE
        android/sysinfo.cpp
        android/jnipp.cpp
        android/JNIHelper.cpp
        )

    target_compile_definitions ( PeripheralPlatform PUBLIC
        -DANDROID_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
        )

    target_compile_options ( PeripheralPlatform PUBLIC
        # This flag gives a lot of false positives for aligned accesses
        -Wno-address-of-packed-member
        )

    target_include_directories ( PeripheralPlatform PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/android/include>
        $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/native_app_glue>
        $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/ndk_helper>
        $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/cpufeatures>
        )
    header_install (
        PeripheralPlatform
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/android/include/coffee/jni>
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
        osx/sysinfo.mm
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
        emscripten/sysinfo.cpp
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
        win32/sysinfo.cpp

#        platforms/win32/file.cpp
        )
    if(WIN_UWP)
        target_sources ( PeripheralPlatform PRIVATE
            win32/wmi_uwp.cpp
            )
    else()
        target_sources ( PeripheralPlatform PRIVATE
            win32/wmi_win32.cpp
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
#        platforms/posix/stacktrace.cpp
        )
endif()
