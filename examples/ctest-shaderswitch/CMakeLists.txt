set ( ShaderSwitch_SRCS
        main.cpp
        )

include_directories(
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

add_executable ( ShaderSwitch ${ShaderSwitch_SRCS} )

target_link_libraries ( ShaderSwitch
        CoffeeCore
        CoffeeGraphicsMisc
        CoffeeOpenGL
    )

target_compile_features(ShaderSwitch PRIVATE cxx_variadic_templates)
