set ( CoffeeAudioTest_SRCS
    main.cpp
    )

coffee_add_example ( CoffeeAudioTest ${CoffeeAudioTest_SRCS} )

target_link_libraries ( CoffeeAudioTest
    CoffeeCore
    CoffeeAudio
    CoffeeOpenGL
    )