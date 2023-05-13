coffee_library(
    TARGET GraphicsDefines
    SOURCES ../private/graphics_defines.cpp
    LIBRARIES
        PeripheralLib
    )
target_include_directories ( GraphicsDefines PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../binding/include>
    )
target_compile_definitions ( GraphicsDefines PUBLIC
    -DGLEAM_USE_CORE
    -DGLEAM_MAX_VERSION=0x460
    -DGLEAM_MAX_VERSION_ES=0
    )
