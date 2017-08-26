find_package (SDL2GameControllerDB REQUIRED)

set ( RSRCS
    ${COFFEE_ASSET_DIRECTORY}/ctest_hud
    ${SDL2_GAMECONTROLLERDB}
    )
set ( LIBS
    CoffeeSDL2
    CoffeeRenderer_Default
    CoffeeImage
    )

coffee_add_application_long (
    SDLSpriteTest "SDL Sprite Test"
    main.cpp
    "${LIBS}" ""
    "${RSRCS}"
    )
