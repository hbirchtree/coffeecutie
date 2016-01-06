set ( AsyncPbo_SRCS
        main.cpp
        )

include_directories(
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

add_executable ( AsyncPbo ${AsyncPbo_SRCS} )

target_link_libraries ( AsyncPbo
        CoffeeCore
        CoffeeGraphicsMisc
        CoffeeOpenGL
    )

target_compile_features(AsyncPbo PRIVATE cxx_variadic_templates)
