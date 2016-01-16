set ( ShaderSwitch_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

coffee_add_example ( ShaderSwitch ${ShaderSwitch_SRCS} )

target_link_libraries ( ShaderSwitch
    CoffeeCore
    CoffeeGraphicsMisc
    CoffeeOpenGL
    )