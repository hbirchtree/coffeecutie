add_library ( GraphicsAPI INTERFACE )

target_include_directories ( GraphicsAPI INTERFACE $<INSTALL_INTERFACE:include> )

header_install (
    GraphicsAPI
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    ""
    )

add_export ( GraphicsAPI "" )

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

if(NOT NO_DYNAMIC_LINKING)
    coffee_library (
        TARGET GpuQuery
        SOURCES private/query/gpu_query.cpp
        LIBRARIES Core
        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        )

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
            GpuQuery
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
            GpuQuery
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
            GpuQuery
            )
    endif()
endif()
