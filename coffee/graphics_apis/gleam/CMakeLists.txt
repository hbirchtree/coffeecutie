set ( CoffeeGLeam_SRCS
    private/renderer/gleamrenderer.cpp
    private/levels/glbase.cpp
    )

if(COFFEE_BUILD_GLEAM_RHI)
    list(APPEND CoffeeGLeam_SRCS
        private/rhi/gleam_rhi.cpp
        private/rhi/gleam_api_rhi.cpp
        private/rhi/gleam_buffer_rhi.cpp
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

coffee_add_library ( CoffeeGLeam "${CoffeeGLeam_SRCS}" )

target_link_libraries ( CoffeeGLeam
    CoffeeCore
    CoffeeImage
    CoffeeSDL2
    )

# For Android, there is some special function loading
    find_package(SDL2 REQUIRED)
    include_directories (${SDL2_INCLUDE_DIR})
    target_link_libraries(CoffeeGLeam ${SDL2_LIBRARY})
