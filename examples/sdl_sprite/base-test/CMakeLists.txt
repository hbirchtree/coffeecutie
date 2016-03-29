list (APPEND LIBS CoffeeSDL2 CoffeeImage)

coffee_add_example ( SDLSpriteTest "SDL Sprite Test" main.cpp "${LIBS}" )