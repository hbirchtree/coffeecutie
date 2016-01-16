set ( MediaTest_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    ${COFFEE_AUDIO_INCLUDE_DIR}
    )

coffee_add_example ( MediaTest ${MediaTest_SRCS} )

target_link_libraries ( MediaTest
    CoffeeCore
    CoffeeOpenGL
    CoffeeGraphicsMisc
    CoffeeFFMedia
    )