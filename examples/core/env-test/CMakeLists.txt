coffee_application (
    TARGET EnvTest
    TITLE "Environment Data"
    SOURCES main.cpp
    LIBRARIES CoffeeCore
    )

if(TARGET CoffeeASIO)
    target_link_libraries ( EnvTest PUBLIC CoffeeASIO )
endif()
