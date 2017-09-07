set ( LIBS CoffeeOpenAL CoffeeRenderer_Default CoffeeGLeamRHI CoffeeCore )

find_package( OpenAL REQUIRED )

coffee_add_application_long ( OpenALAudioTest
    "OpenAL Audio Test" main.cpp
    "${LIBS}" "${OPENAL_LIBRARY}"
    ${COFFEE_ASSET_DIRECTORY}/caudio_test
    )
