find_package ( stb REQUIRED )

coffee_library (
    TARGET Image

    LIBRARIES Core

    SOURCES
    private/cimage.cpp
    private/cnoise-gen.cpp
    private/ctextrender.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( Image SYSTEM PRIVATE
    $<BUILD_INTERFACE:${STB_INCLUDE_DIR}>
    )
