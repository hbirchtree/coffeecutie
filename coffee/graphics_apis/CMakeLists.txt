if(COFFEE_BUILD_GLBINDING)
    add_subdirectory(opengl)
endif()

# GLeam requires no dependencies outside of SDL2
if(COFFEE_BUILD_GLEAM)
    add_subdirectory(gleam)
endif()

add_definitions(-DNVIDIA_NVML_AVAILABLE)

include_directories("/home/havard/nvidia-gdk/usr/include/nvidia/gdk")

coffee_add_library ( CoffeeGraphicsAPI "private/nullrenderer.cpp;../core/private/plat/graphics/gpu/gpu_query.cpp" )

target_link_libraries( CoffeeGraphicsAPI
    CoffeeCore
    )

coffee_add_elibrary ( "coffee-nvidia" SHARED "../core/private/plat/graphics/gpu/nvidia_query.cpp" )
#coffee_add_elibrary ( "coffee-amd" SHARED "" )
#coffee_add_elibrary ( "coffee-intel" SHARED "" )

target_link_libraries( "coffee-nvidia"
    "/usr/lib/nvidia-370/libnvidia-ml.so"
    )
