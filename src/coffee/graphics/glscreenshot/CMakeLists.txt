coffee_library (
    TARGET GLScreenshot
    SOURCES private/screenshot.cpp
    LIBRARIES ComponentApp GLeamCommon
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

if(BUILD_GLES)
    target_compile_definitions ( GLScreenshot PRIVATE
        -DGLADCOMP_COMPILE_ES
        )
else()
    target_compile_definitions ( GLScreenshot PRIVATE
        -DGLADCOMP_COMPILE_CORE
        )
endif()
