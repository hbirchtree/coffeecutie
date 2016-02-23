if(COFFEE_BUILD_GLBINDING)
    add_subdirectory(opengl)
endif()

# GLeam requires no dependencies outside of SDL2
add_subdirectory(gleam)

set ( CoffeeGraphicsMisc_SRCS
    )

coffee_add_library ( CoffeeGraphicsMisc "${CoffeeGraphicsMisc_SRCS}" )

target_link_libraries ( CoffeeGraphicsMisc
        CoffeeCore
    )
