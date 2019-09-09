coffee_library (
    TARGET GLADComponent
    SOURCES private/glad_comp.cpp
    LIBRARIES ComponentApp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

if(BUILD_GLES AND NOT BUILD_GLES_20)
    target_link_libraries ( GLADComponent PUBLIC
        GLAD_ES
        )
    target_compile_definitions ( GLADComponent PRIVATE
        -DGLADCOMP_COMPILE_ES
        )
elseif(NOT BUILD_GLES)
    target_link_libraries ( GLADComponent PUBLIC
        GLAD_Core
        )
    target_compile_definitions ( GLADComponent PRIVATE
        -DGLADCOMP_COMPILE_CORE
        )
endif()
