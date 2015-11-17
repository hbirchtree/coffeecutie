set ( EnvTest_SRCS
        main.cpp
        )

add_executable ( EnvTest ${EnvTest_SRCS} )

target_link_libraries ( EnvTest
CoffeeCore
)

target_compile_features(EnvTest PRIVATE cxx_variadic_templates)
