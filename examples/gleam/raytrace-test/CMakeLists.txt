set ( LIBS CoffeeCore CoffeeImage CoffeeRenderer_Default CoffeeCore )

coffee_add_application ( RaytraceTest "Raytracing" main.cpp "${LIBS}" )
