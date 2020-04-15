coffee_library(
    TARGET EGLComponent
    SOURCES private/egl_comp.cpp
    LIBRARIES ComponentApp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

if(BUILD_GLKIT)
    target_link_libraries ( EGLComponent PUBLIC CEAGL )
else()
    target_link_libraries ( EGLComponent PUBLIC EGL )
endif()
