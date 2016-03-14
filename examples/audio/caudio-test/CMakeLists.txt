set ( CoffeeAudioTest_SRCS
    main.cpp
    )

coffee_add_example ( OpenALAudioTest ${CoffeeAudioTest_SRCS} )

target_link_libraries ( OpenALAudioTest
    CoffeeCore
    CoffeeOpenAL
    CoffeeOpenGL
    )
