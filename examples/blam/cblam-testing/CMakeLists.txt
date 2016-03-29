list (APPEND LIBS CoffeeCore CoffeeBlam)

coffee_add_example ( BlamTesting "Blam Loader" main.cpp "${LIBS}" )