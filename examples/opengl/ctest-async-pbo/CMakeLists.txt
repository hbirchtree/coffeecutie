set ( AsyncPbo_SRCS
    main.cpp
    )

coffee_add_example ( AsyncPbo ${AsyncPbo_SRCS} )

target_link_libraries ( AsyncPbo
    CoffeeCore
    CoffeeOpenGL
    )