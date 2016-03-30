list (APPEND LIBS CoffeeCore CoffeeOpenAL CoffeeSDL2)

if(ANDROID)
    find_package( OpenAL REQUIRED )
    coffee_add_example ( OpenALAudioTest "OpenAL Audio Test" main.cpp "${LIBS}" "${OPENAL_LIBRARY}" )
else()
    coffee_add_example ( OpenALAudioTest "OpenAL Audio Test" main.cpp "${LIBS}" )
endif()
