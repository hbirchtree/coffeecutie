coffee_application (
    TARGET TextRenderTest
    TITLE "Text Rendering"
    SOURCES main.cpp
    LIBRARIES CoffeeImage CoffeeCore
    )

target_include_directories( TextRenderTest
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src/libs/stb
    )
