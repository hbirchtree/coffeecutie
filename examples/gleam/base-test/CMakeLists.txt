set ( GLeamBaseTest_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_INCLUDE_DIR}
    ${COFFEE_OPENVR_INCLUDE_DIR}
    )

coffee_add_example ( GLeamBaseTest ${GLeamBaseTest_SRCS} )

target_link_libraries ( GLeamBaseTest
    CoffeeCore
    CoffeeVR
    CoffeeGraphicsMisc
    CoffeeGLeam
    )