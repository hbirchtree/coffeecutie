set (DynamicLib_SRCS
    dynatest.cpp
    )

coffee_add_elibrary ( DynamicLib SHARED ${DynamicLib_SRCS} )

coffee_add_example ( DynamicTest "Dynamic Loading Test" main.cpp CoffeeCore )

target_link_libraries ( DynamicLib
    CoffeeCore
    CoffeeOpenGL
    CoffeeImage
    CoffeeOpenAL
    )
