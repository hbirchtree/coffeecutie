set ( CBlamTesting_SRCS
    main.cpp
    )

coffee_add_example ( BlamTesting ${CBlamTesting_SRCS} )

target_link_libraries ( BlamTesting
    CoffeeCore
    CoffeeBlam
    )