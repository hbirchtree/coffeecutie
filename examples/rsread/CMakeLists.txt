set ( RSReadTest_SRCS
    main.cpp
    )

coffee_add_example ( RSReadTest ${RSReadTest_SRCS} )

target_link_libraries ( RSReadTest
    CoffeeCore
    )