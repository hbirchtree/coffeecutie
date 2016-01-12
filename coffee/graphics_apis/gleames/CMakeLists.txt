include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_SDL2_INCLUDE_DIR}
    )

set ( CoffeeGLeamES_SRCS
                private/renderer/GLeamESrenderer.cpp
                private/levels/glbase.cpp

                private/glad/glad.c
    )

add_library( CoffeeGLeam ${COFFEE_LINK_OPT} ${CoffeeGLeamES_SRCS} )

target_link_libraries ( CoffeeGLeam
                CoffeeCore
                CoffeeImage
                CoffeeSDL2
        )

set_property(TARGET CoffeeGLeam PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeGLeam PRIVATE cxx_variadic_templates)
