set ( TemplateTest_SRCS
        ${COFFEE_EXAMPLE_DIR}/template-test/main.cpp
        )

add_executable ( TemplateTest ${TemplateTest_SRCS} )

target_link_libraries ( TemplateTest
CoffeeCore
)

target_compile_features(TemplateTest PRIVATE cxx_variadic_templates)
