find_package (SDL2GameControllerDB REQUIRED)

list (APPEND RSRCS
    ${CMAKE_SOURCE_DIR}/sample_data/ctest_hud
    ${SDL2_GAMECONTROLLERDB}
    )
list (APPEND LIBS
    CoffeeSDL2
    CoffeeImage
    )

coffee_add_example_long (
    SDLSpriteTest "SDL Sprite Test"
    main.cpp
    "${LIBS}" ""
    "${RSRCS}"
    )
