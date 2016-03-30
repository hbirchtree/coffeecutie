list ( APPEND LIBRARIES
    CoffeeCore
    CoffeeOpenGL
    CoffeeGLeam
 )

coffee_add_example ( CoffeeTestHud "HUD Rendering Example" main.cpp "${LIBRARIES}" )
