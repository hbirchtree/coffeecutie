coffee_library (
    TARGET ComponentApp
    SOURCES
    private/subsystems.cpp
    private/comp_app.cpp

    LIBRARIES
    EntComp
    Core

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
