set ( GLeamBaseTest_SRCS
    main.cpp
    )

coffee_add_example ( GLeamBaseTest ${GLeamBaseTest_SRCS} )

target_link_libraries ( GLeamBaseTest
    CoffeeGLeam
    CoffeeOVR
    )