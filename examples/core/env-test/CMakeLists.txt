set ( EnvTest_SRCS
    main.cpp
    )

coffee_add_example ( EnvTest ${EnvTest_SRCS} )

target_link_libraries ( EnvTest
    CoffeeCore
    CoffeeImage
    )
