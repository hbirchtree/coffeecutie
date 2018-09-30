option ( BUILD_GLES "Build library targeting OpenGL ES 3.0+" OFF )
option ( BUILD_GLES_20 "Build library targeting OpenGL ES 2.0" OFF )

if(WIN32 AND WIN_UWP)
    option ( BUILD_ANGLE "Build with OpenGL ES 2.0 backed by ANGLE" ON )
endif()

coffee_library (
    TARGET CoffeeGLeamCommon

    SOURCES
    private/levels/glbase.cpp
    private/levels/shared/gl_shared_debug.cpp

    LIBRARIES
    CoffeeCore
    CoffeeImage
    CoffeeGraphicsAPI

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/coffee/windowing/private/graphics/ceagl/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/toolchain/internal/include>
    )

if(BUILD_GLEAM)
    coffee_library (
        TARGET CoffeeGLeamRHI
        SOURCES
        private/rhi/gleam_rhi.cpp

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
        CoffeeGLeamCommon

        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
        )
endif()

if(BUILD_ANGLE)
    find_package( ANGLE REQUIRED )
    list ( APPEND CORE_EXTRA_LIBRARIES
        AngleEGL AngleGLESv2
        )
elseif(BUILD_GLES AND NOT APPLE)
    if(ANDROID AND NOT BUILD_GLES_20)
        # On Android, GLES 3.x is used through a different library
        target_link_libraries ( CoffeeGLeamCommon PUBLIC
            GLESv3
            )
    else()
        # Elsewhere, just use GLESv2
        target_link_libraries ( CoffeeGLeamCommon PUBLIC
            GLESv2
            )
    endif()

    if(MAEMO)
        target_link_libraries ( CoffeeGLeamCommon PUBLIC
            IMGegl
            )
    endif()
endif()

if(BUILD_GLES_20 OR BUILD_ANGLE)
    target_compile_definitions ( CoffeeGLeamCommon PUBLIC
        -DCOFFEE_GLES20_MODE
        )
elseif(NOT BUILD_GLES)
    target_compile_definitions ( CoffeeGLeamCommon PUBLIC
        -DCOFFEE_GLEAM_DESKTOP
        )
endif()
