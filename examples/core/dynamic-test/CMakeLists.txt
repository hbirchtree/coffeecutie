set ( DynamicTest_SRCS
    main.cpp
    )

set (DynamicLib_SRCS
    dynatest.cpp
    )

coffee_add_elibrary ( DynamicLib SHARED ${DynamicLib_SRCS} )
coffee_add_example ( DynamicTest ${DynamicTest_SRCS} )

target_link_libraries ( DynamicTest
    CoffeeCore
    )
target_link_libraries ( DynamicLib
    CoffeeCore
    CoffeeOpenGL
    CoffeeImage
    CoffeeOpenAL
    )
