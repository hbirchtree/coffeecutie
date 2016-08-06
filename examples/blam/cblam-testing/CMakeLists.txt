list (APPEND LIBS CoffeeCore CoffeeBlam)

coffee_add_application ( BlamTesting "Blam Loader" main.cpp "${LIBS}" )