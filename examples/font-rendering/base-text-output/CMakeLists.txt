set ( LIBS CoffeeImage CoffeeCore )

coffee_add_application ( TextRenderTest "Text Rendering" main.cpp "${LIBS}" )
