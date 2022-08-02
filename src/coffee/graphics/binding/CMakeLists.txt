coffee_library (
    TARGET GLAD_Core
    SOURCES glad_core/glad.c
    LIBRARIES CoreCommon
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad_core/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad_core/include/glad>
    $<INSTALL_INTERFACE:include>
    )

coffee_library (
    TARGET GLAD_ES
    SOURCES glad_es/glad.c
    LIBRARIES CoreCommon
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad_es/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad_es/include/glad>
    $<INSTALL_INTERFACE:include>
    )
