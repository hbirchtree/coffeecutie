find_package (SDL2GameControllerDB REQUIRED)

list (APPEND RSRCS
    ${COFFEE_ASSET_DIRECTORY}/ctest_hud
    ${SDL2_GAMECONTROLLERDB}
    )
list (APPEND LIBS
    CoffeeSDL2
    CoffeeImage
    )

coffee_add_application_long (
    SDLSpriteTest "SDL Sprite Test"
    main.cpp
    "${LIBS}" ""
    "${RSRCS}"
    )
