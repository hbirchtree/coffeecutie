find_package ( stb REQUIRED )

coffee_library (
    TARGET CoffeeImage

    SOURCES
    private/cimage.cpp
    private/cnoise-gen.cpp
    private/ctextrender.cpp

    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/image
    )

target_include_directories ( CoffeeImage
    PRIVATE SYSTEM
    ${STB_INCLUDE_DIR}
    )
