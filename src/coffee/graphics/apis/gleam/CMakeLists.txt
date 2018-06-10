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
    ${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/gleam/levels
    )

target_include_directories ( CoffeeGLeamCommon
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/coffee/windowing/private/graphics/ceagl/include>
    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( CoffeeGLeamCommon
    PUBLIC
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
        private/rhi/gleam_surface_rhi.cpp
        private/rhi/gleam_vertex_rhi.cpp

        LIBRARIES
        CoffeeGLeamCommon

        HEADER_DIRS
        ${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/gleam/rhi
        )
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
