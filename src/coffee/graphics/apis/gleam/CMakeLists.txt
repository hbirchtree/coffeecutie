if(COFFEE_BUILD_GLEAM)
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
        PRIVATE
        ${CMAKE_SOURCE_DIR}/coffee/windowing/private/graphics/ceagl/include
        )

    target_include_directories ( CoffeeGLeamCommon
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/toolchain/internal/include>
        )

    if(COFFEE_BUILD_GLEAM_RHI)
        coffee_library (
            TARGET CoffeeGLeamRHI
            SOURCES
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

            LIBRARIES
            CoffeeGLeamCommon
            
            HEADER_DIRS
            ${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/gleam/rhi
            )
    endif()
endif()
