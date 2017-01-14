set ( CoffeeSDL2_SRCS
    private/sdl2system.cpp

    private/windowing/csdl2_dialogs.cpp
    private/graphics/csdl2_basic_renderer.cpp
    private/graphics/csdl2_gl_renderer.cpp
    private/input/csdl2_eventhandler.cpp
    private/windowing/csdl2_window.cpp

    private/input/evhandlers/sdl2eventhandlers.cpp
    private/windowing/sdl2helpers.cpp
    )

set ( CoffeeSDL2Audio_SRCS
    private/audio/csdl2_audio.cpp
    )

find_package ( SDL2 REQUIRED)

#if(NOT SDL2_FOUND)
#    message(FATAL_ERROR "Failed to locate SDL2")
#endif()

# Emscripten has an SDL2 port
if(NOT EMSCRIPTEN)
    include_directories ( SYSTEM
        ${SDL2_INCLUDE_DIR}
        )

    set ( LIBS
        CoffeeCore
        ${SDL2_LIBRARY}
        )
endif()

coffee_add_library ( CoffeeSDL2 "${CoffeeSDL2_SRCS}" "${LIBS}"
    "${CMAKE_SOURCE_DIR}/include/coffee/sdl2")
