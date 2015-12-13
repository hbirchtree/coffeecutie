set ( CoffeeAudioTest_SRCS
        main.cpp
    )

include_directories(
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_AUDIO_INCLUDE_DIR}
    )

add_executable ( CoffeeAudioTest ${CoffeeAudioTest_SRCS} )

target_link_libraries ( CoffeeAudioTest
        CoffeeCore
        CAudio
        CoffeeOpenGL
    )

target_compile_features(CoffeeAudioTest PRIVATE cxx_variadic_templates)
