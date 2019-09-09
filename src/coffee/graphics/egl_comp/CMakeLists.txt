coffee_library(
    TARGET EGLComponent
    SOURCES private/egl_comp.cpp
    LIBRARIES ComponentApp EGL
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
