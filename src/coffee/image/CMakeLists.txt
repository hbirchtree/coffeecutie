find_package ( Stb REQUIRED )

coffee_library (
    TARGET Image

    LIBRARIES Core

    SOURCES
    private/cimage.cpp
    private/ctextrender.cpp
#    private/cnoise-gen.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( Image SYSTEM PRIVATE
    $<BUILD_INTERFACE:${Stb_INCLUDE_DIR}>
    )
