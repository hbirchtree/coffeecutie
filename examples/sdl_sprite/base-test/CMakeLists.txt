find_package (SDL2GameControllerDB REQUIRED)

coffee_application (
    TARGET SDLSpriteTest
    TITLE "SDL Sprite Test"

    SOURCES main.cpp

    LIBRARIES
    SDL2Backend
    RendererDefault
    Image

    RESOURCES
    ${COFFEE_ASSET_DIRECTORY}/ctest_hud
    ${SDL2_GAMECONTROLLERDB}
    )
