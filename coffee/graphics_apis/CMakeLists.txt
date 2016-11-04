if(COFFEE_BUILD_GLBINDING)
    add_subdirectory(opengl)
endif()

# GLeam requires no dependencies outside of SDL2
if(COFFEE_BUILD_GLEAM)
    add_subdirectory(gleam)
endif()

coffee_add_library ( CoffeeGraphicsAPI "private/nullrenderer.cpp;../core/private/plat/graphics/gpu/gpu_query.cpp" )

target_link_libraries( CoffeeGraphicsAPI
    CoffeeCore
    )

find_package( NvidiaML QUIET )
if(NVIDIAML_FOUND)
    coffee_add_elibrary ( "coffee-nvidia" SHARED "../core/private/plat/graphics/gpu/nvidia_query.cpp" )

    include_directories( "${NVIDIAML_INCLUDE_DIR}" )
    target_link_libraries( "coffee-nvidia"
        "${NVIDIAML_LIBRARIES}"
        )
endif()

#coffee_add_elibrary ( "coffee-amd" SHARED "" )
#coffee_add_elibrary ( "coffee-intel" SHARED "" )
