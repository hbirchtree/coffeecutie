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

add_library ( GLAD INTERFACE )

if(BUILD_GLES AND NOT BUILD_GLES_20)
    target_link_libraries ( GLAD INTERFACE GLAD_ES )
elseif(NOT BUILD_GLES)
    target_link_libraries ( GLAD INTERFACE GLAD_Core )
endif()

add_export ( GLAD "" )
