set ( CoffeeAudioTest_SRCS
        main.cpp
        )

include_directories(
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/opengl/include
        ${CMAKE_SOURCE_DIR}/coffee/audio/include
    )

add_executable ( CoffeeAudioTest ${CoffeeAudioTest_SRCS} )

target_link_libraries ( CoffeeAudioTest
CoffeeCore
CAudio
CoffeeOpenGL
)

target_compile_features(CoffeeAudioTest PRIVATE cxx_variadic_templates)
