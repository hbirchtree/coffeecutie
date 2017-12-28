set ( LIBS CoffeeCore )

if(TARGET CoffeeASIO)
    set ( LIBS "CoffeeASIO;CoffeeCore" )
endif()

coffee_add_application ( EnvTest "Environment Data" main.cpp "${LIBS}" )

if(TARGET CoffeeASIO)
    target_compile_definitions( EnvTest
        PUBLIC
        -DUSE_ASIO
        )
endif()
