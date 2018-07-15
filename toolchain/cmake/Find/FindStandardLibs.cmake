# Some variables
set ( CORE_INCLUDE_DIR )
set ( CORE_EXTRA_LIBRARIES )

# Platform-specific target options

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    # Libunwind is used to print function names at runtime
    # Windows does not support this library
    # Android's Bionic does not grant access to unwind, even though it is there. Bad Google, bad!
    if(NOT DEFINED LIBUNWIND_INCLUDE_DIR)
        find_package(Unwind QUIET )
    endif()
    if (LIBUNWIND_FOUND)
        list ( APPEND CORE_INCLUDE_DIR
            $<BUILD_INTERFACE:${LIBUNWIND_INCLUDE_DIR}>
            )
        list ( APPEND CORE_EXTRA_LIBRARIES ${LIBUNWIND_LIBRARIES} )
    endif()

    if(BUILD_GLES)
        list ( APPEND CORE_EXTRA_LIBRARIES EGL GLESv2 )
    endif()
endif()

if(BUILD_SDL2)
    if(SDL_POWER_PLUGIN_ENABLED OR ANDROID OR WIN_UWP)
        # We use SDL2 for some platform functionality, like power info
        # On Android, it is also used to read assets and
        #  acquiring device info.
        if(NOT DEFINED SDL2_INCLUDE_DIR)
            find_package(SDL2 REQUIRED)
        endif()
        if(SDL2_INCLUDE_DIR)
            list ( APPEND CORE_INCLUDE_DIR
                $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
                )
            if(NOT ${SDL2_LIBRARY} MATCHES ".framework")
                list ( APPEND CORE_INCLUDE_DIR
                    $<INSTALL_INTERFACE:include/SDL2>
                    )
            endif()
        endif()
        #    list ( APPEND CORE_EXTRA_LIBRARIES ${SDL2_LIBRARY} )
    endif()
endif()

if(APPLE)
    list ( APPEND CORE_EXTRA_LIBRARIES
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework Foundation"
        #        "-framework OpenAL"
        "-framework QuartzCore"
        )
    if(IOS)
        list ( APPEND CORE_EXTRA_LIBRARIES
            "-framework CoreMotion"
            "-framework GameController"
            "-framework OpenGLES"
            "-framework UIKit"
            "-framework GLKit"
            )
    else()
        list ( APPEND CORE_EXTRA_LIBRARIES
            "-framework OpenGL"
            "-framework AppKit"
            )
    endif()
endif()

if(ANDROID)
    # Add the Android logging library, as well as android and GLESv*
    list ( APPEND CORE_EXTRA_LIBRARIES
        # Logging and Android functions
        log android
        # All Android targets use EGL
        EGL
        )

    # If we are building a GLESv3-exclusive target, drop GLESv2
    if(BUILD_GLES_20)
        list ( APPEND CORE_EXTRA_LIBRARIES
            GLESv2
            )
    else()
        list ( APPEND CORE_EXTRA_LIBRARIES
            GLESv3
            )
    endif()

    if(BUILD_SDL2)
        list ( APPEND CORE_EXTRA_LIBRARIES
            SDL2
            )
    endif()
endif()

if(RASPBERRY)
    # Linkage on Raspberry kind of looks like Android
    # We also have bcm_host for accessing OpenGL for some reason
    # Next we might look for OpenMAX?
    list ( APPEND CORE_EXTRA_LIBRARIES
        #        GLESv2
        bcm_host

        #        asound
        #        pulse-simple pulse
        #        X11 Xext Xcursor Xinerama Xi Xrandr Xss Xxf86vm xkbcommon
        #        wayland-egl wayland-client wayland-cursor
        )
endif()

if(WIN32)
    # Don't know what this is, but it's necessary
    if(NOT WIN_UWP)
        list ( APPEND CORE_EXTRA_LIBRARIES
            # For some of the file API
            # This one is not available on UWP
            #pathcch

            # Core includes
            user32
            gdi32
            winmm
            imm32
            ole32
            oleaut32
            shell32
            version
            ws2_32
            wbemuuid
            )
    else()
        list ( APPEND CORE_EXTRA_LIBRARIES
            OneCore
            )
    endif()

    if(MINGW64)
        list ( APPEND CORE_EXTRA_LIBRARIES
            mingw32
            )
    endif()

    list ( APPEND CORE_EXTRA_LIBRARIES
        cabinet
        )

    if(BUILD_ANGLE)
        find_package( ANGLE REQUIRED )
        list ( APPEND CORE_EXTRA_LIBRARIES
            AngleEGL AngleGLESv2
            )
    endif()
endif()

if(MAEMO)
    list ( APPEND CORE_EXTRA_LIBRARIES
        GLESv2 IMGegl
        )
endif()

if(GAMECUBE)
    list ( APPEND CORE_EXTRA_LIBRARIES
        ogc
        )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND NOT ANDROID)
    # Used for thread details
    list ( APPEND CORE_EXTRA_LIBRARIES rt )
endif()

if(NOT WIN32 AND NOT MINGW AND NOT MSYS AND NOT EMSCRIPTEN)
    # Necessary for Linux and possibly OS X (latter is untested)
    if(NOT GAMECUBE)
        list ( APPEND CORE_EXTRA_LIBRARIES dl z )
    endif()
    list ( APPEND CORE_EXTRA_LIBRARIES m)
endif()

if(NOT WIN32 AND NOT APPLE AND NOT EMSCRIPTEN AND NOT GAMECUBE)
    find_package(Threads REQUIRED)
    list( APPEND CORE_EXTRA_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
endif()

set ( STANDARDLIBS_LIBRARIES "${CORE_EXTRA_LIBRARIES}" CACHE STRING "" )
set ( STANDARDLIBS_INCLUDE_DIR "${CORE_INCLUDE_DIR}" CACHE STRING "" )

message ( STATUS "Standardlibs in use: ${STANDARDLIBS_LIBRARIES}" )
message ( STATUS "Standard includes in use: ${STANDARDLIBS_INCLUDE_DIR}" )
