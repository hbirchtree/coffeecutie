set ( RaytraceTest_SRCS
    main.cpp
    )

coffee_add_example ( RaytraceTest ${RaytraceTest_SRCS} )

target_link_libraries ( RaytraceTest
    CoffeeCore
    CoffeeGraphicsMisc
    CoffeeImage
    CoffeeGLeam
    )
