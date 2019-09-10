coffee_library (
    TARGET AppDelegate
    SOURCES
    private/AppDelegate.m
    ${CEAGL_DIR}/private/eagl_bind.mm
    ${CEAGL_DIR}/private/EGLView.m

    LIBRARIES
    PeripheralPlatform
    "-framework CoreGraphics"
    "-framework CoreMotion"
    "-framework GameController"
    "-framework GLKit"
    "-framework OpenGLES"
    "-framework QuartzCore"
    "-framework UIKit"

    HEADER_DIRS
    $<BUILD_INTERFACE:${CEAGL_DIR}/include>
    )

coffee_library (
    TARGET GLKitComponent
    SOURCES private/glk_comp.cpp
    LIBRARIES ComponentApp AppDelegate
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
