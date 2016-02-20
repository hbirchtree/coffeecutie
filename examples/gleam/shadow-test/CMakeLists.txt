set ( GLeamShadowTest_SRCS
    main.cpp
    )

coffee_add_example ( GLeamShadowTest ${GLeamShadowTest_SRCS} )

target_link_libraries ( GLeamShadowTest
    CoffeeGraphicsMisc
    CoffeeGLeam
    )