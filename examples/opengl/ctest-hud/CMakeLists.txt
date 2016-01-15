set ( CoffeeTestHud_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

add_executable ( CoffeeTestHud ${CoffeeTestHud_SRCS} )

target_link_libraries ( CoffeeTestHud
    CoffeeCore
    CoffeeGraphicsMisc
    CoffeeOpenGL
    CoffeeGLeam
    )

target_enable_cxx11(CoffeeTestHud)