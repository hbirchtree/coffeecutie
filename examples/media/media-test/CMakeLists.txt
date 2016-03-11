set ( MediaTest_SRCS
    main.cpp
    )

coffee_add_example ( MediaTest ${MediaTest_SRCS} )

target_link_libraries ( MediaTest
    CoffeeCore
    CoffeeOpenGL
    CoffeeFFMedia
    )