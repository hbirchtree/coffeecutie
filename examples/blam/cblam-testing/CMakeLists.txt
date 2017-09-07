set ( LIBS CoffeeBlam CoffeeCore )

coffee_add_application ( BlamTesting "Blam Loader" main.cpp "${LIBS}" )
