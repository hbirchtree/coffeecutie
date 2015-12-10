set ( CoffeeAudioTest_SRCS
        main.cpp
        )

add_executable ( CoffeeAudioTest ${CoffeeAudioTest_SRCS} )

target_link_libraries ( CoffeeAudioTest
CoffeeCore
CAudio
CoffeeOpenGL
)

target_compile_features(CoffeeAudioTest PRIVATE cxx_variadic_templates)
