set ( GLeamBaseTest_SRCS
        main.cpp
        )

coffee_add_example ( Template ${GLeamBaseTest_SRCS} )

target_link_libraries ( Template
        CoffeeCore
        CoffeeGLeam
    )
