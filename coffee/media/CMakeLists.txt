include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_AUDIO_INCLUDE_DIR}
    )

set ( CoffeeFFMedia_SRCS
        private/cffmedia.cpp
        )

add_library( CoffeeFFMedia ${COFFEE_LINK_OPT} ${CoffeeFFMedia_SRCS})

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

set_property(TARGET CoffeeFFMedia PROPERTY POSITION_INDEPENDENT_CODE ON)
target_enable_cxx11(CoffeeFFMedia)
