set ( TemplateTest_SRCS
        main.cpp
        )

include_directories(
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

add_executable ( TemplateTest ${TemplateTest_SRCS} )

target_link_libraries ( TemplateTest
		CoffeeCore
        CoffeeGraphicsMisc
        CoffeeOpenGL
	)

target_compile_features(TemplateTest PRIVATE cxx_variadic_templates)
