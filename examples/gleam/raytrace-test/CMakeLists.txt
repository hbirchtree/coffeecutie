list (APPEND LIBS CoffeeCore CoffeeImage CoffeeGLeam)

coffee_add_application ( RaytraceTest "Raytracing" main.cpp "${LIBS}" )