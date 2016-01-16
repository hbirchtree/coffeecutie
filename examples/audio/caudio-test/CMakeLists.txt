set ( CoffeeAudioTest_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_GRAPHICS_GLEAM_INCLUDE_DIR}
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    ${COFFEE_AUDIO_INCLUDE_DIR}
    )

coffee_add_example ( CoffeeAudioTest ${CoffeeAudioTest_SRCS} )

target_link_libraries ( CoffeeAudioTest
    CoffeeCore
    CoffeeAudio
    CoffeeOpenGL
    )