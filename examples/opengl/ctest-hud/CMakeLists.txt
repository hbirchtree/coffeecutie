set ( CoffeeTestHud_SRCS
    main.cpp
    )

coffee_add_example ( CoffeeTestHud ${CoffeeTestHud_SRCS} )

target_link_libraries ( CoffeeTestHud
    CoffeeCore
    CoffeeOpenGL
    CoffeeGLeam
    )