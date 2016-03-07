if(COFFEE_BUILD_GLBINDING)
    add_subdirectory(opengl)
endif()

# GLeam requires no dependencies outside of SDL2
if(COFFEE_BUILD_GLEAM)
    add_subdirectory(gleam)
endif()

set ( CoffeeGraphicsMisc_SRCS
    )

coffee_add_library ( CoffeeGraphicsMisc "${CoffeeGraphicsMisc_SRCS}" )

target_link_libraries ( CoffeeGraphicsMisc
        CoffeeCore
    )
