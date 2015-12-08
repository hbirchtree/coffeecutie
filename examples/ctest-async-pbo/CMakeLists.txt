set ( AsyncPbo_SRCS
        main.cpp
        )

add_executable ( AsyncPbo ${AsyncPbo_SRCS} )

target_link_libraries ( AsyncPbo
CoffeeCore
)

target_compile_features(AsyncPbo PRIVATE cxx_variadic_templates)
