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

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

find_package( NvidiaML QUIET )
if(NVIDIAML_FOUND)
    coffee_add_elibrary ( "coffee-nvidia" SHARED "../core/private/plat/graphics/gpu/nvidia_query.cpp" )

    add_library( nvidia-ml SHARED IMPORTED )
    set_property(TARGET nvidia-ml PROPERTY IMPORTED_LOCATION "${NVIDIAML_LIBRARIES}" )

    include_directories( "${NVIDIAML_INCLUDE_DIR}" )
    target_link_libraries( "coffee-nvidia"
        nvidia-ml
        )
endif()

#coffee_add_elibrary ( "coffee-amd" SHARED "" )
#coffee_add_elibrary ( "coffee-intel" SHARED "" )
