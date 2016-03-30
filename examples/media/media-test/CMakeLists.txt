list ( APPEND LIBRARIES
    CoffeeCore
    CoffeeOpenGL
    CoffeeFFMedia
 )

coffee_add_example ( MediaTest "FFMPEG test" main.cpp "${LIBRARIES}" )
