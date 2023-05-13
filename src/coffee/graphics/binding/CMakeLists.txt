coffee_library(
    TARGET GLAD
    SOURCES
        private/egl.c
    LIBRARIES
        CoreCommon
    HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

if(BUILD_GLES)
    target_sources(GLAD PRIVATE
        private/gles2.c
        )
else()
    target_sources(GLAD PRIVATE
        private/gl.c
        )
endif()
