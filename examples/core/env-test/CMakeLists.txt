list (APPEND LIBS CoffeeCore)

coffee_add_application ( EnvTest "Environment Data" main.cpp "CoffeeCore;CoffeeGraphicsAPI" )
