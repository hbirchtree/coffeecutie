coffee_library (
    TARGET GLAD_Core
    SOURCES glad_core/glad.c
    LIBRARIES CoreCommon
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad_core/include>
    $<INSTALL_INTERFACE:include>
    )

coffee_library (
    TARGET GLAD_ES
    SOURCES glad_es/glad.c
    LIBRARIES CoreCommon
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad_es/include>
    $<INSTALL_INTERFACE:include>
    )
