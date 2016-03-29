list (APPEND LIBS CoffeeCore CoffeeImage CoffeeGLeam)

coffee_add_example ( RaytraceTest "Raytracing" main.cpp "${LIBS}" )