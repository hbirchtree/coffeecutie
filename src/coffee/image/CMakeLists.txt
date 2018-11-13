find_package ( stb REQUIRED )

coffee_library (
    TARGET CoffeeImage

    LIBRARIES CoffeeCore

    SOURCES
    private/cimage.cpp
    private/cnoise-gen.cpp
    private/ctextrender.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( CoffeeImage SYSTEM PRIVATE
    $<BUILD_INTERFACE:${STB_INCLUDE_DIR}>
    )
