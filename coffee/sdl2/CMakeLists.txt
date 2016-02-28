set ( CoffeeSDL2_SRCS
    private/csdl2renderer.cpp

    private/sdl2system.cpp

    private/windowing/csdl2_dialogs.cpp
    private/graphics/csdl2_gl_renderer.cpp
    private/input/csdl2_eventhandler.cpp
    private/windowing/csdl2_window.cpp

    private/input/evhandlers/sdl2eventhandlers.cpp
    private/windowing/sdl2helpers.cpp
    )

find_package ( SDL2 REQUIRED)

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "Failed to locate SDL2")
endif()

include_directories ( SYSTEM
    ${SDL2_INCLUDE_DIR}
    )

coffee_add_library ( CoffeeSDL2 "${CoffeeSDL2_SRCS}" )

target_link_libraries( CoffeeSDL2
    CoffeeCore
    ${SDL2_LIBRARY}
    )