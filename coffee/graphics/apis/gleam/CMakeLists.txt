set ( CoffeeGLeam_SRCS
    private/levels/glbase.cpp
    private/levels/shared/gl_shared_debug.cpp
    )

set ( CoffeeGLeamRHI_SRCS
    private/rhi/gleam_rhi.cpp
    private/rhi/gleam_api_query.cpp
    private/rhi/gleam_api_rhi.cpp
    private/rhi/gleam_buffer_rhi.cpp
    private/rhi/gleam_framebuffer_rhi.cpp
    private/rhi/gleam_profiler_rhi.cpp
    private/rhi/gleam_query_rhi.cpp
    private/rhi/gleam_quad_draw.cpp
    private/rhi/gleam_shader_rhi.cpp
    private/rhi/gleam_surface_rhi.cpp
    private/rhi/gleam_vertex_rhi.cpp
    )

set ( LIBS
    CoffeeImage
    CoffeeGraphicsAPI
    )

if(COFFEE_BUILD_GLEAM)
    coffee_add_library ( CoffeeGLeamCommon "${CoffeeGLeam_SRCS}" "${LIBS}"
        "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/gleam/levels"
        )

    target_link_libraries ( CoffeeGLeamCommon
        PRIVATE
        CoffeeCore
        )

    if(COFFEE_BUILD_GLEAM_RHI)
        coffee_add_library ( CoffeeGLeamRHI "${CoffeeGLeamRHI_SRCS}" "${LIBS};CoffeeGLeamCommon"
            "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/gleam/rhi"
            )
        target_link_libraries ( CoffeeGLeamRHI
            PRIVATE
            CoffeeCore
            )
    endif()
endif()

#set ( RendererGL_SRCS
#    private/renderers/gl_lib.cpp
#    )
#set ( RendererGLES_SRCS
#    private/renderers/gles_lib.cpp
#    )
#set ( RendererEGL_SRCS
#    private/renderers/egl_lib.cpp
#    )

#coffee_add_library ( GLeam_Renderer_GL "${RendererGL_SRCS}" "CoffeeGLeam;CoffeeSDL2"
#    "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/renderers/gl_lib" )

#coffee_add_library ( GLeam_Renderer_GLES "${RendererGL_SRCS}" "CoffeeGLeam;CoffeeSDL2"
#    "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/renderers/gles_lib" )

#coffee_add_library ( GLeam_Renderer_EGL "${RendererGL_SRCS}" "CoffeeGLeam;CoffeeSDL2"
#    "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/renderers/egl_lib" )
