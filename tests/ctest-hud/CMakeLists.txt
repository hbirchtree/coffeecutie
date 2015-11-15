set ( CoffeeTestHud_SRCS
        ${COFFEE_EXAMPLE_DIR}/ctest-hud/main.cpp
        )

add_executable ( CoffeeTestHud ${CoffeeTestHud_SRCS} )

target_link_libraries ( CoffeeTestHud
CoffeeCore
)

target_compile_features(CoffeeTestHud PRIVATE cxx_variadic_templates)
