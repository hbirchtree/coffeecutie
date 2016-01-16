include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_AUDIO_INCLUDE_DIR}
    )

set ( CoffeeFFMedia_SRCS
        private/cffmedia.cpp
        )

coffee_add_library ( CoffeeFFMedia "${CoffeeFFMedia_SRCS}" )

target_link_libraries( CoffeeFFMedia
        CoffeeCore
        CoffeeOpenGL
        CoffeeAudio

        avutil
        swresample
        swscale
        avcodec
        avformat
        avdevice
    )