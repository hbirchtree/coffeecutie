option ( BUILD_GLES "Build library targeting OpenGL ES 3.0+" OFF )
option ( BUILD_GLES_20 "Build library targeting OpenGL ES 2.0" OFF )

if(WIN32 AND WIN_UWP)
    option ( BUILD_ANGLE "Build with OpenGL ES 2.0 backed by ANGLE" ON )
endif()

add_library ( GLVersion INTERFACE )

if(BUILD_GLES_20 OR BUILD_ANGLE)
    target_compile_definitions ( GLVersion INTERFACE
        -DGLEAM_USE_ES
        -DGLEAM_USE_LINKED
        )
elseif(BUILD_GLES)
    target_compile_definitions ( GLVersion INTERFACE
        -DGLEAM_USE_ES
        -DGLEAM_USE_LINKED
        )
elseif(NOT BUILD_GLES)
    target_compile_definitions ( GLVersion INTERFACE
        -DGLEAM_USE_CORE
        )
endif()

add_export ( GLVersion "" )

coffee_library (
    TARGET GLeamCommon

    SOURCES
    private/levels/shared/gl_shared_debug.cpp

    LIBRARIES
    Core
    Image
    GraphicsAPI
    GLVersion

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( GLeamCommon PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/coffee/windowing/private/graphics/ceagl/include>
    )

if(BUILD_GLEAM)
    coffee_library (
        TARGET GLeamRHI
        SOURCES

        private/rhi/gleam_api_query.cpp
        private/rhi/gleam_api_rhi.cpp
        private/rhi/gleam_api_draw.cpp

        private/rhi/gleam_buffer_rhi.cpp

        private/rhi/gleam_framebuffer_rhi.cpp

        private/rhi/gleam_profiler_rhi.cpp
        private/rhi/gleam_query_rhi.cpp
        private/rhi/gleam_quad_draw.cpp

        private/rhi/gleam_shader_rhi.cpp
        private/rhi/gleam_shader_uniforms.cpp

        private/rhi/gleam_sampler_rhi.cpp
        private/rhi/gleam_surface_rhi.cpp

        private/rhi/gleam_vertex_rhi.cpp

        LIBRARIES
        GLeamCommon

        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
        )
endif()

if(APPLE AND IOS)
    target_compile_definitions ( GLeamCommon PUBLIC
        -DGLES_SILENCE_DEPRECATION=1
        )
    target_link_libraries ( GLeamCommon PUBLIC
        CEAGL
        )
endif()

if(BUILD_ANGLE)
    # TODO: Write wrapper for importing/downloading choco packages
    find_package( ANGLE CONFIG REQUIRED )
    list ( APPEND CORE_EXTRA_LIBRARIES
        AngleEGL AngleGLESv2
        )
elseif(BUILD_GLES AND NOT APPLE)
    if(ANDROID AND NOT BUILD_GLES_20)
        # On Android, GLES 3.x is used through a different library
        target_link_libraries ( GLeamCommon PUBLIC
            GLESv3
            )
    else()
        find_library ( GLESV2_LIB GLESv2 )

        # Elsewhere, just use GLESv2
        target_link_libraries ( GLeamCommon PUBLIC
            ${GLESV2_LIB}
            )
    endif()

    if(MAEMO)
        target_link_libraries ( GLeamCommon PUBLIC
            IMGegl
            )
    endif()
    if(BEAGLE AND FALSE)
        set ( BEAGLE_SUFFIXES
            lib
            lib/${CMAKE_LIBRARY_ARCHITECTURE}
            )

        find_library ( IMGEGL_LIB IMGegl
            PATH_SUFFIXES ${BEAGLE_SUFFIXES}
            PATHS
            ${DEB_SYSROOT}/usr
            )
        find_library ( SRVUM_LIB srv_um
            PATH_SUFFIXES ${BEAGLE_SUFFIXES}
            PATHS
            ${DEB_SYSROOT}/usr
            )
        find_library ( DRMOMAP_LIB
            NAMES drm_omap libdrm_omap.so.1
            PATH_SUFFIXES ${BEAGLE_SUFFIXES}
            PATHS
            ${DEB_SYSROOT}/usr
            )
        find_library ( DRM_LIB
            NAMES drm libdrm.so.2
            PATH_SUFFIXES ${BEAGLE_SUFFIXES}
            PATHS
            ${DEB_SYSROOT}/usr
            )
        find_library ( WLSERVER_LIB
            NAMES wayland-server  libwayland-server.so.0
            PATH_SUFFIXES ${BEAGLE_SUFFIXES}
            PATHS
            ${DEB_SYSROOT}/usr
            )
        find_library ( WLCLIENT_LIB
            NAMES wayland-client libwayland-client.so.0
            PATH_SUFFIXES ${BEAGLE_SUFFIXES}
            PATHS
            ${DEB_SYSROOT}/usr
            )
        find_library ( FFI_LIB
            NAMES ffi libffi.so.6
            PATH_SUFFIXES ${BEAGLE_SUFFIXES}
            PATHS
            ${DEB_SYSROOT}/usr
            )

        target_link_libraries ( GLeamCommon PUBLIC
            ${IMGEGL_LIB} ${SRVUM_LIB}
            ${DRMOMAP_LIB} ${WLSERVER_LIB} ${WLCLIENT_LIB}

            ${DRM_LIB} ${FFI_LIB}
            )
    endif()
elseif(APPLE AND IOS)
    target_link_libraries ( GLeamCommon PUBLIC
        "-framework OpenGLES"
        )
endif()

if(BUILD_GLES_20 OR BUILD_ANGLE)
    target_compile_definitions ( GLeamCommon PUBLIC
        -DCOFFEE_LINKED_GLES
        -DCOFFEE_GLES20_MODE
        )
elseif(BUILD_GLES)
    target_compile_definitions ( GLeamCommon PUBLIC
        -DCOFFEE_LINKED_GLES
        -DCOFFEE_LINKED_GLES31
        -DCOFFEE_LINKED_GLES30
        )
elseif(NOT BUILD_GLES)
    target_compile_definitions ( GLeamCommon PUBLIC
        -DCOFFEE_GLEAM_DESKTOP
        )
endif()

if(NOT BUILD_GLES)
    coffee_bundle (
        DESTINATION ""
        HEADER_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/gl_include/glad"
        )
    target_include_directories ( GLeamCommon PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gl_include>
        )
else()
    coffee_bundle (
        DESTINATION ""
        HEADER_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/gles_include/glad_es"
        )
    target_include_directories ( GLeamCommon PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gles_include>
        )
endif()

if( TARGET  )

endif()
