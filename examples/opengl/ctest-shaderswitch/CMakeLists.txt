set ( ShaderSwitch_SRCS
    main.cpp
    )

coffee_add_example ( ShaderSwitch ${ShaderSwitch_SRCS} )

target_link_libraries ( ShaderSwitch
    CoffeeCore
    CoffeeGraphicsMisc
    CoffeeOpenGL
    )