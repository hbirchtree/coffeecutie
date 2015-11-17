set ( TemplateTest_SRCS
        main.cpp
        )

add_executable ( CGameBoyTest ${TemplateTest_SRCS} )

target_link_libraries ( CGameBoyTest
CoffeeCore
)

target_compile_features(CGameBoyTest PRIVATE cxx_variadic_templates)
