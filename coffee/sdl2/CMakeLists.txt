set ( CoffeeSDL2_SRCS
    private/csdl2renderer.cpp
    private/sdl2eventhandlers.cpp
    private/sdl2helpers.cpp
    )

find_package(SDL2 REQUIRED)

include_directories(SYSTEM
    ${SDL2_INCLUDE_DIR}
    )

include_directories(
    ${COFFEE_SDL2_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

coffee_add_library ( CoffeeSDL2 "${CoffeeSDL2_SRCS}" )

target_link_libraries( CoffeeSDL2
    CoffeeCore
    ${SDL2_LIBRARY}
    )