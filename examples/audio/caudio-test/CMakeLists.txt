find_package( OpenAL REQUIRED )

coffee_application (
    TARGET OpenALAudioTest
    TITLE "OpenAL Audio Test"
    SOURCES main.cpp

    LIBRARIES
    OpenALMix DropinRenderer GLeamRHI Core

    BUNDLE_LIBRARIES ${OPENAL_LIBRARY}

    RESOURCES ${COFFEE_ASSET_DIRECTORY}/caudio_test
    )
