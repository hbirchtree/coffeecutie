option ( BUILD_GLES "Build library targeting OpenGL ES 3.0+" OFF )
option ( BUILD_GLES_20 "Build library targeting OpenGL ES 2.0" OFF )

if(WIN32 AND WIN_UWP)
    option ( BUILD_ANGLE "Build with OpenGL ES 2.0 backed by ANGLE" ON )
endif()

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_library ( GLVersion_Dynamic   INTERFACE )
    target_compile_definitions ( GLVersion_Dynamic INTERFACE
        -DGLEAM_USE_CORE
        -DGLEAM_MAX_VERSION=0x460
        -DGLEAM_MAX_VERSION_ES=0
        )
    target_link_libraries ( GLVersion_Dynamic INTERFACE GLAD_Core )
    add_export ( GLVersion_Dynamic "" )
endif()
if (NOT (APPLE AND NOT IOS))
    add_library ( GLVersion_ES        INTERFACE )
    if(EMSCRIPTEN)
        target_compile_definitions ( GLVersion_ES INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_USE_LINKED
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=0x300
            )
    else()
        target_compile_definitions ( GLVersion_ES INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_USE_LINKED
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=0x320
            )
    endif()
    add_export ( GLVersion_ES "" )

    add_library ( GLVersion_ES2 INTERFACE )
    target_compile_definitions ( GLVersion_ES2 INTERFACE
        -DGLEAM_USE_ES
        -DGLEAM_USE_LINKED
        -DGLEAM_MAX_VERSION=0
        -DGLEAM_MAX_VERSION_ES=0x200
        -DGLEAM_RESTRICT_ES=0x200
        )
    add_export ( GLVersion_ES2 "" )
    if (NOT APPLE)
        add_library ( GLVersion_ESDynamic INTERFACE )
        target_compile_definitions ( GLVersion_ESDynamic INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=0x320
            )
        target_link_libraries ( GLVersion_ESDynamic INTERFACE GLAD_ES )
        add_export ( GLVersion_ESDynamic "" )
        add_library ( GLVersion_ES2Dynamic INTERFACE )
        target_compile_definitions ( GLVersion_ES2Dynamic INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=0x200
            -DGLEAM_RESTRICT_ES=0x200
            )
        target_link_libraries ( GLVersion_ES2Dynamic INTERFACE GLAD_ES )
        add_export ( GLVersion_ES2Dynamic "" )
    endif()
endif()

function ( GEN_GLEAM_TARGET GLVERSION SUFFIX )
    coffee_library (
        TARGET GLeamCommon_${SUFFIX}

        SOURCES
        private/glw.cpp
    #    private/levels/shared/gl_shared_debug.cpp

        LIBRARIES
        Core
        Image
        GraphicsAPI
        GraphicsInterface
        ${GLVERSION}

        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        $<INSTALL_INTERFACE:include>
        )


    if(BUILD_GLEAM)
        coffee_library (
            TARGET GLeamRHI_${SUFFIX}
            SOURCES

            private/rhi/rhi.cpp

    #        private/rhi/gleam_api_query.cpp
#            private/rhi/gleam_api_rhi.cpp
    #        private/rhi/gleam_api_draw.cpp

    #        private/rhi/gleam_buffer_rhi.cpp

    #        private/rhi/gleam_framebuffer_rhi.cpp

    #        private/rhi/gleam_profiler_rhi.cpp
    #        private/rhi/gleam_query_rhi.cpp
    #        private/rhi/gleam_quad_draw.cpp

    #        private/rhi/gleam_shader_rhi.cpp
    #        private/rhi/gleam_shader_uniforms.cpp

    #        private/rhi/gleam_sampler_rhi.cpp
    #        private/rhi/gleam_surface_rhi.cpp

    #        private/rhi/gleam_vertex_rhi.cpp

            LIBRARIES
            GLeamCommon_${SUFFIX}

            HEADER_DIRS
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            )
    endif()

    if (${GLVERSION} MATCHES "_Dynamic")
        target_include_directories ( GLeamCommon_${SUFFIX} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gl_include>
            )
    elseif (${GLVERSION} MATCHES "_ES2?Dynamic")
        target_include_directories ( GLeamCommon_${SUFFIX} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gles_include>
            )
    endif()

    if(APPLE AND IOS AND ${GLVERSION} MATCHES "_ES")
        target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC "-framework OpenGLES" )
        target_compile_definitions ( GLeamCommon_${SUFFIX} PUBLIC
            -DGLES_SILENCE_DEPRECATION=1
            )
        target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
            CEAGL
            )
        target_include_directories ( GLeamCommon_${SUFFIX} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/coffee/windowing/private/graphics/ceagl/include>
            )
    elseif(BUILD_ANGLE AND ${GLVERSION} MATCHES "_ES")
        find_package( ANGLE CONFIG REQUIRED )
        target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC AngleEGL AngleGLESv2 )
    elseif(APPLE AND ${GLVERSION} MATCHES "_Dynamic")
        target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC "-framework OpenGL" )
    elseif(${GLVERSION} MATCHES "_ES")
        if(ANDROID AND NOT BUILD_GLES_20)
            # On Android, GLES 3.x is used through a different library
            target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
                GLESv3
                )
        elseif(EMSCRIPTEN)
            target_link_options ( GLeamCommon_${SUFFIX} PUBLIC
                "-s    " MAX_WEBGL_VERSION=2
                )
        else()
            find_library ( GLESV2_LIB GLESv2 )

            # Elsewhere, just use GLESv2
            target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
                ${GLESV2_LIB}
                )
        endif()

        if(MAEMO)
            target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
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

            target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
                ${IMGEGL_LIB} ${SRVUM_LIB}
                ${DRMOMAP_LIB} ${WLSERVER_LIB} ${WLCLIENT_LIB}

                ${DRM_LIB} ${FFI_LIB}
                )
        endif()
    endif()
endfunction()

foreach ( VER Dynamic ES ES2 ESDynamic ES2Dynamic )
    if (NOT TARGET GLVersion_${VER} )
        continue()
    endif()
    gen_gleam_target ( GLVersion_${VER} ${VER} )
endforeach()

if ( TARGET GLVersion_Dynamic )
    coffee_bundle (
        DESTINATION ""
        HEADER_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/gl_include/glad"
        )
endif()
if ( TARGET GLVersion_ESDynamic OR TARGET GLVersion_ES2Dynamic )
    coffee_bundle (
        DESTINATION ""
        HEADER_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/gles_include/glad_es"
        )
endif()
