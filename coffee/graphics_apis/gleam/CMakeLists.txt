include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_SDL2_INCLUDE_DIR}
    )

set ( CoffeeGLeam_SRCS
                private/renderer/gleamrenderer.cpp
                private/levels/glbase.cpp

                private/glad/glad.c
    )

coffee_add_library ( CoffeeGLeam "${CoffeeGLeam_SRCS}" )

target_link_libraries ( CoffeeGLeam
                CoffeeCore
                CoffeeImage
                CoffeeSDL2
        )