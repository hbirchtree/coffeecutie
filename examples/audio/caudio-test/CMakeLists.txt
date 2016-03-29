list (APPEND LIBS CoffeeCore CoffeeOpenAL CoffeeSDL2)

coffee_add_example ( OpenALAudioTest "OpenAL Audio Test" main.cpp "${LIBS}" )
