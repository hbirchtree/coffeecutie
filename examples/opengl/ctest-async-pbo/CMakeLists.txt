set ( AsyncPbo_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

coffee_add_example ( AsyncPbo ${AsyncPbo_SRCS} )

target_link_libraries ( AsyncPbo
    CoffeeCore
    CoffeeGraphicsMisc
    CoffeeOpenGL
    )