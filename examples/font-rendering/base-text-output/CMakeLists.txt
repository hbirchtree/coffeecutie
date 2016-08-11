list (APPEND LIBS CoffeeImage )

coffee_add_application ( TextRenderTest "Text Rendering" main.cpp "${LIBS}" )
