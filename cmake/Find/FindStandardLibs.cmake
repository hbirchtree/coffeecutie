# Some variables
set ( CORE_INCLUDE_DIR )
set ( CORE_EXTRA_LIBRARIES )

# Platform-specific target options

if(NOT WIN32 AND NOT ANDROID)
    # Used for thread details
    # Might replace this with Thread
    list ( APPEND CORE_EXTRA_LIBRARIES pthread )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND NOT ANDROID AND NOT NACL)
    # Used for thread details
    list ( APPEND CORE_EXTRA_LIBRARIES rt )
endif()

if(NOT WIN32 AND NOT MINGW AND NOT MSYS AND NOT NACL)
    # Necessary for Linux and possibly OS X (latter is untested)
    list ( APPEND CORE_EXTRA_LIBRARIES dl m )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    # Libunwind is used to print function names at runtime
    # Windows does not support this library
    # Android's Bionic does not grant access to unwind, even though it is there. Bad Google, bad!
    find_package(Unwind QUIET )
    if (LIBUNWIND_FOUND)
        list ( APPEND CORE_INCLUDE_DIR
            ${LIBUNWIND_INCLUDE_DIR}
            )
        list ( APPEND CORE_EXTRA_LIBRARIES ${LIBUNWIND_LIBRARIES} )
    endif()

    if(NOT NACL)
        list ( APPEND CORE_EXTRA_LIBRARIES X11 )

        if(COFFEE_BUILD_GLES)
            list ( APPEND CORE_EXTRA_LIBRARIES EGL )
        endif()

        if(NOT COFFEE_BUILD_GLES)
            list ( APPEND CORE_EXTRA_LIBRARIES
                GL Xrender
                )
        endif()
    endif()
endif()

if( SDL_POWER_PLUGIN_ENABLED OR ANDROID OR EMSCRIPTEN)
    # We use SDL2 for some platform functionality, like power info
    # On Android, it is also used to read assets and
    #  acquiring device info.
    find_package(SDL2 REQUIRED)
    list ( APPEND CORE_INCLUDE_DIR
        ${SDL2_INCLUDE_DIR}
        )
#    list ( APPEND CORE_EXTRA_LIBRARIES ${SDL2_LIBRARY} )
endif()

if(APPLE)
    list ( APPEND CORE_EXTRA_LIBRARIES
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework Foundation"
        "-framework OpenAL"
        "-framework QuartzCore"
        )
    if(IOS)
        list ( APPEND CORE_EXTRA_LIBRARIES
            "-framework CoreMotion"
            "-framework GameController"
            "-framework OpenGLES"
            "-framework UIKit"
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
        GLESv2
        EGL

        ${SDL2_LIBRARY}
        ${SDL2_LIBRARIES}
        )
    list ( APPEND CORE_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/bindings/android/include
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
        GLESv2
        bcm_host

#        asound
#        pulse-simple pulse
#        X11 Xext Xcursor Xinerama Xi Xrandr Xss Xxf86vm xkbcommon
#        wayland-egl wayland-client wayland-cursor
        )
endif()

if(WIN32)
    # Don't know what this is, but it's necessary
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
         Ws2_32
         wbemuuid
          )

	if(COFFEE_BUILD_ANGLE)
		find_package( ANGLE REQUIRED )
		list ( APPEND CORE_EXTRA_LIBRARIES
			${ANGLE_LIBRARIES}
			)
		list ( APPEND CORE_INCLUDE_DIR
			${ANGLE_INCLUDE_DIR}
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

set ( STANDARDLIBS_LIBRARIES "${CORE_EXTRA_LIBRARIES}" CACHE STRING "" )
set ( STANDARDLIBS_INCLUDE_DIR "${CORE_INCLUDE_DIR}" CACHE STRING "" )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(StandardLibs
    REQUIRED_VARS
    STANDARDLIBS_LIBRARIES
)
