project ( GLeamBaseTest )

coffee_application (
    TARGET GLeamBaseTest_RHI
    TITLE "GLeam Basic RHI"

    SOURCES
    main.cpp
    renderer.h

    LIBRARIES
    CoffeeGLeamRHI
    CoffeeRenderer_Default
    CoffeeCore

    RESOURCES
    "${COFFEE_ASSET_DIRECTORY}/eye-demo/"

    USE_ASIO
    )
