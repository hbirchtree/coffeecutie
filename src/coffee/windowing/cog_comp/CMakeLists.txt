coffee_library (
    TARGET CogComponent
    SOURCES private/cog_comp.cpp
    LIBRARIES ComponentApp Gexxo
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
