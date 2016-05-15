set ( LIBS CoffeeCore CoffeeOpenAL CoffeeSDL2 )

find_package( OpenAL REQUIRED )
coffee_add_example_long ( OpenALAudioTest "OpenAL Audio Test" main.cpp "${LIBS}" "${OPENAL_LIBRARY}" "" )
