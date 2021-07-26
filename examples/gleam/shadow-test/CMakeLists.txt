coffee_application (
    TARGET GLeamShadowTest
    TITLE "GLeam Shadows"
    SOURCES main.cpp
    LIBRARIES
    DropinRenderer
    GLeamRHI_Dynamic
    Core
    )

if(TARGET ASIO)
    target_link_libraries ( GLeamShadowTest PUBLIC
        ASIO
        )
endif()

