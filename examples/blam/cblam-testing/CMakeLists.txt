coffee_application (
    TARGET BlamTesting
    TITLE "Blam Loader"
    SOURCES main.cpp
    LIBRARIES
    CoffeeBlam CoffeeCore CoffeeImage
    CoffeeRenderer_Default CoffeeGLeamRHI
    )
