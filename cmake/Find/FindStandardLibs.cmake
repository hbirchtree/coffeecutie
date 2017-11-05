# Some variables
set ( CORE_INCLUDE_DIR )
set ( CORE_EXTRA_LIBRARIES )

# Platform-specific target options

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    # Libunwind is used to print function names at runtime
    # Windows does not support this library
    # Android's Bionic does not grant access to unwind, even though it is there. Bad Google, bad!
    find_package(Unwind QUIET )
    if (LIBUNWIND_FOUND)
        list ( APPEND CORE_INCLUDE_DIR
            $<BUILD_INTERFACE:${LIBUNWIND_INCLUDE_DIR}>
            )
        list ( APPEND CORE_EXTRA_LIBRARIES ${LIBUNWIND_LIBRARIES} )
    endif()

    if(NOT NACL)
        if(COFFEE_BUILD_GLES)
            list ( APPEND CORE_EXTRA_LIBRARIES EGL GLESv2 )
        endif()
    endif()
endif()

if(COFFEE_BUILD_SDL2)
    if(SDL_POWER_PLUGIN_ENABLED OR ANDROID OR WIN_UWP)
        # We use SDL2 for some platform functionality, like power info
        # On Android, it is also used to read assets and
        #  acquiring device info.
        find_package(SDL2 REQUIRED)
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
            )
    endif()
endif()

if(ANDROID)
    # Add the Android logging library, as well as android and GLESv*
    list ( APPEND CORE_EXTRA_LIBRARIES
        # Logging and Android functions
        log android
        # OpenGL ES
        #        GLESv1_CM
#        GLESv2
        EGL
        )

    if(COFFEE_BUILD_GLES_20)
        list ( APPEND CORE_EXTRA_LIBRARIES
            GLESv2
            )
    else()
        list ( APPEND CORE_EXTRA_LIBRARIES
            GLESv3
            )
    endif()

    if(COFFEE_BUILD_SDL2)
        list ( APPEND CORE_EXTRA_LIBRARIES
            SDL2
            ${SDL2_LIBRARIES}
            )
    endif()

    list ( APPEND CORE_INCLUDE_DIR
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/bindings/android/include>
        )
    #    if("${ANDROID_NATIVE_API_LEVEL}" GREATER 17)
    #        message ( "-- Building with GLES 3.0+ support" )
    #        list ( APPEND CORE_EXTRA_LIBRARIES
    #                GLESv3
    #                )
    #    endif()
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

    if(COFFEE_BUILD_ANGLE)
        find_package( ANGLE REQUIRED )
        list ( APPEND CORE_EXTRA_LIBRARIES
            AngleEGL AngleGLESv2
            )
    endif()
endif()

if(NACL)
    find_package(SDL2 REQUIRED)
    list ( APPEND CORE_EXTRA_LIBRARIES
        ppapi_simple
        ppapi
        nacl_io
        nosys
        )
endif()

if(MAEMO)
    list ( APPEND CORE_EXTRA_LIBRARIES
        GLESv2 IMGegl
        )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND NOT ANDROID AND NOT NACL)
    # Used for thread details
    list ( APPEND CORE_EXTRA_LIBRARIES rt )
endif()

if(NOT WIN32 AND NOT MINGW AND NOT MSYS AND NOT NACL AND NOT EMSCRIPTEN)
    # Necessary for Linux and possibly OS X (latter is untested)
    list ( APPEND CORE_EXTRA_LIBRARIES dl m z )
endif()

if(NOT WIN32 AND NOT APPLE AND NOT EMSCRIPTEN)
    find_package(Threads REQUIRED)
    list( APPEND CORE_EXTRA_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
endif()

set ( STANDARDLIBS_LIBRARIES "${CORE_EXTRA_LIBRARIES}" CACHE STRING "" )
set ( STANDARDLIBS_INCLUDE_DIR "${CORE_INCLUDE_DIR}" CACHE STRING "" )

message ( STATUS "Standardlibs in use: ${STANDARDLIBS_LIBRARIES}" )
message ( STATUS "Standard includes in use: ${STANDARDLIBS_INCLUDE_DIR}" )
