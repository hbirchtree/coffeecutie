include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

set ( CoffeeSDL2_SRCS
        private/csdl2renderer.cpp
        private/sdl2eventhandlers.cpp
        private/sdl2helpers.cpp
        )

find_package(SDL2 REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIR}
    ${COFFEE_SDL2_INCLUDE_DIR}
    )

add_library( CoffeeSDL2 ${COFFEE_LINK_OPT} ${CoffeeSDL2_SRCS})

target_link_libraries( CoffeeSDL2
        CoffeeCore
        ${SDL2_LIBRARY}
    )

set_property(TARGET CoffeeSDL2 PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeSDL2 PRIVATE cxx_variadic_templates)
