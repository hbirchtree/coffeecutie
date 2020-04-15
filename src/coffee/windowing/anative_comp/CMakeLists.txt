coffee_library (
    TARGET ANativeComponent
    SOURCES private/anative_comp.cpp
    LIBRARIES ComponentApp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
