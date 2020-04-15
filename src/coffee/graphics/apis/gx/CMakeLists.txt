option ( BUILD_GX2 "Target GX2 API" OFF )

coffee_library (
    TARGET Gexxo
    SOURCES
    private/gexxo_api.cpp

    LIBRARIES
    Core
    Image

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
