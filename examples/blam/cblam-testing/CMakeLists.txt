set ( CBlamTesting_SRCS
    main.cpp
    )

coffee_add_example ( CBlamTesting ${CBlamTesting_SRCS} )

target_link_libraries ( CBlamTesting
    CoffeeCore
    CoffeeBlam
    )

target_enable_cxx11(CBlamTesting)
