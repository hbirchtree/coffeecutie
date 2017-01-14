set ( CoffeeGLeam_SRCS
    private/renderer/gleamrenderer.cpp
    private/levels/glbase.cpp

    private/csdl2renderer.cpp
    )

if(COFFEE_BUILD_GLEAM_RHI)
    list(APPEND CoffeeGLeam_SRCS
        private/rhi/gleam_rhi.cpp
        private/rhi/gleam_api_rhi.cpp
        private/rhi/gleam_buffer_rhi.cpp
        private/rhi/gleam_framebuffer_rhi.cpp
	private/rhi/gleam_profiler_rhi.cpp
        private/rhi/gleam_query_rhi.cpp
        private/rhi/gleam_shader_rhi.cpp
        private/rhi/gleam_surface_rhi.cpp
        private/rhi/gleam_vertex_rhi.cpp
        )
endif()

if(COFFEE_BUILD_GLES)
    add_definitions(-DCOFFEE_ESMODE)
    list(APPEND CoffeeGLeam_SRCS private/glad_es/glad.c)
else()
    list(APPEND CoffeeGLeam_SRCS private/glad/glad.c)
endif()

set ( LIBS
    CoffeeCore
    CoffeeImage
    CoffeeSDL2
    CoffeeGraphicsAPI
    )

if(ANDROID)

endif()

# For Android, there is some special function loading
find_package(SDL2 REQUIRED)
if(NOT EMSCRIPTEN)
    include_directories (${SDL2_INCLUDE_DIR})
endif()

list ( APPEND LIBS ${SDL2_LIBRARY})

coffee_add_library ( CoffeeGLeam "${CoffeeGLeam_SRCS}" "${LIBS}"
    "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis/gleam")
