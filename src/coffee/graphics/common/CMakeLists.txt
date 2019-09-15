coffee_library (
    TARGET GraphicsAPI
    SOURCES private/query/gpu_query.cpp
    LIBRARIES Core
    )

target_include_directories ( GraphicsAPI PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
    )

coffee_bundle (
    HEADER_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

find_package( NvidiaML QUIET )

find_package( LibDRM QUIET )

if(NVIDIAML_FOUND)
    coffee_library (
        TARGET NVIDIAQuery
        LINKAGE SHARED
        SOURCES
        private/query/nvidia_query.cpp

        LIBRARIES
        ${NVIDIAML_LIBRARIES}
        Core
        GraphicsAPI
        )

    target_include_directories( NVIDIAQuery PRIVATE
        $<BUILD_INTERFACE:${NVIDIAML_INCLUDE_DIR}>
        )
endif()

if(LIBDRM_FOUND AND FALSE)
    coffee_library (
        TARGET LibDRMQuery
        LINKAGE SHARED
        SOURCES
        private/query/libdrm_query.cpp
        private/query/libdrm_query.c

        LIBRARIES
        ${LIBDRM_LIBRARIES}
        Core
        )

    target_include_directories( LibDRMQuery PRIVATE
        $<BUILD_INTERFACE:${LIBDRM_INCLUDE_DIR}>
        )
endif()

if(BUILD_OPENGL AND BUILD_GLEAM AND NOT EMSCRIPTEN AND NOT ANDROID AND NOT APPLE)
    coffee_library (
        TARGET OpenGLQuery
        LINKAGE SHARED
        NO_EXPORT
        SOURCES private/query/opengl_query.cpp

        LIBRARIES
        Core
        GLAD
        GLeamCommon
        GraphicsAPI
        )
endif()
