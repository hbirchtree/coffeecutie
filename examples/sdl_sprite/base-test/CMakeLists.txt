find_package (SDL2GameControllerDB REQUIRED)

coffee_application (
    TARGET SDLSpriteTest
    TITLE "SDL Sprite Test"

    SOURCES main.cpp

    LIBRARIES
    CoffeeSDL2
    CoffeeRenderer_Default
    CoffeeImage

    RESOURCES
    ${COFFEE_ASSET_DIRECTORY}/ctest_hud
    ${SDL2_GAMECONTROLLERDB}
    )
