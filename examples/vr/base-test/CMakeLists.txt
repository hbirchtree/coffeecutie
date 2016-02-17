set ( VRBaseTest_SRCS
    main.cpp
    )

coffee_add_example ( VRBaseTest ${VRBaseTest_SRCS} )

target_link_libraries ( VRBaseTest
    CoffeeVR
    )
