coffee_application (
    TARGET TextRenderTest
    TITLE "Text Rendering"
    SOURCES main.cpp
    LIBRARIES Image Core
    )

target_include_directories( TextRenderTest
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src/libs/stb
    )
