find_package ( X11 REQUIRED )

coffee_library (
    TARGET X11Component
    SOURCES private/x11_comp.cpp
    LIBRARIES ComponentApp ${X11_LIBRARIES} Xrender GL
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
