set ( CoffeeAudioTest_SRCS
        ${COFFEE_EXAMPLE_DIR}/caudio-test/main.cpp
        )

add_executable ( CoffeeAudioTest ${CoffeeAudioTest_SRCS} )

target_link_libraries ( CoffeeAudioTest
CoffeeCore
CAudio
)

target_compile_features(CoffeeAudioTest PRIVATE cxx_variadic_templates)
