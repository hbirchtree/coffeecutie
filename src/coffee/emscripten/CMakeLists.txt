coffee_library (
    TARGET EmscriptenComponents
    SOURCES
    private/emscripten_components.cpp

    LIBRARIES
    ComponentApp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
