set ( MediaTest_SRCS
        main.cpp
        )

add_executable ( MediaTest ${MediaTest_SRCS} )

target_link_libraries ( MediaTest
        CoffeeCore
        CoffeeFFMedia
        )

target_compile_features(MediaTest PRIVATE cxx_variadic_templates)
