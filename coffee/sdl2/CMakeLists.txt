set ( CoffeeSDL2_SRCS
    private/sdl2system.cpp

    private/windowing/csdl2_dialogs.cpp

    private/graphics/csdl2_basic_renderer.cpp

    private/graphics/csdl2_gl_renderer.cpp

    private/input/csdl2_eventhandler.cpp

    private/windowing/csdl2_window.cpp

    private/windowing/sdl2helpers.cpp
    private/input/evhandlers/sdl2eventhandlers.cpp
    )

set ( CoffeeX11_SRCS
    private/windowing/x11_window.cpp
    )

set ( CoffeeDMX_SRCS
    private/windowing/dispmanx_window.cpp
    )

set ( CoffeeEGL_SRCS
    private/graphics/egl_renderer.cpp
    )

set ( CoffeeGLX_SRCS
    private/graphics/glx_renderer.cpp
    )

set ( CoffeeSDL2Audio_SRCS
    private/audio/csdl2_audio.cpp
    )

find_package ( SDL2 REQUIRED)

include_directories ( SYSTEM
    ${SDL2_INCLUDE_DIR}
	${STANDARDLIBS_INCLUDE_DIR}
    )

coffee_add_library ( CoffeeWindow_X11 "${CoffeeX11_SRCS}" "CoffeeCore"
    "${CMAKE_SOURCE_DIR}/include/coffee/sdl2"
    )

coffee_add_library ( CoffeeWindow_DMX "${CoffeeDMX_SRCS}" "CoffeeCore"
    "${CMAKE_SOURCE_DIR}/include/coffee/sdl2"
    )

coffee_add_library ( CoffeeContext_GLX "${CoffeeGLX_SRCS}" "CoffeeCore"
    "${CMAKE_SOURCE_DIR}/include/coffee/sdl2"
    )

coffee_add_library ( CoffeeContext_EGL "${CoffeeEGL_SRCS}" "CoffeeCore"
    "${CMAKE_SOURCE_DIR}/include/coffee/sdl2"
    )

set ( LIBS
    CoffeeCore
    ${SDL2_LIBRARY}
    CoffeeWindow_X11
    CoffeeWindow_DMX
    CoffeeContext_GLX
    CoffeeContext_EGL
    ${SDL2_LIBRARIES}
    )

coffee_add_library ( CoffeeSDL2 "${CoffeeSDL2_SRCS}" "${LIBS}"
    "${CMAKE_SOURCE_DIR}/include/coffee/sdl2")

coffee_bundle_includes( ${SDL2_INCLUDE_DIR} )
coffee_bundle_library( ${SDL2_LIBRARY} )
