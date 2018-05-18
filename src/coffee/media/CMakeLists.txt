set ( CoffeeFFMedia_SRCS private/cffmedia.cpp )

coffee_add_library ( CoffeeFFMedia "${CoffeeFFMedia_SRCS}" )

target_link_libraries( CoffeeFFMedia
#        CoffeeCore
#        CoffeeOpenGL
	CoffeeOpenAL

        avutil
        swresample
        swscale
        avcodec
        avformat
        avdevice
    )
