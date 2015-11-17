set ( CoffeeTestHud_SRCS
        main.cpp
        )

add_executable ( CoffeeTestHud ${CoffeeTestHud_SRCS} )

target_link_libraries ( CoffeeTestHud
CoffeeCore
CImage
)

target_compile_features(CoffeeTestHud PRIVATE cxx_variadic_templates)
