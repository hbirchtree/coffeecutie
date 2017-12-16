set ( LIBS CoffeeBlam CoffeeCore CoffeeImage CoffeeRenderer_Default CoffeeGLeamRHI )

coffee_add_application ( BlamTesting "Blam Loader" main.cpp "${LIBS}" )
