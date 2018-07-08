find_package ( stb REQUIRED )

coffee_library (
    TARGET CoffeeImage

    LIBRARIES CoffeeCore

    SOURCES
    private/cimage.cpp
    private/cnoise-gen.cpp
    private/ctextrender.cpp
    )

target_include_directories ( CoffeeImage SYSTEM PRIVATE
    $<BUILD_INTERFACE:${STB_INCLUDE_DIR}>
    )
