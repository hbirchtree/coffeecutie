if(COFFEE_BUILD_GLBINDING)
    add_subdirectory(opengl)
endif()

if(COFFEE_BUILD_GLES)
    add_subdirectory(gleames)
else()
    add_subdirectory(gleam)
endif()

set ( CoffeeGraphicsMisc_SRCS
        private/scene/cnode.cpp
    )

coffee_add_library ( CoffeeGraphicsMisc "${CoffeeGraphicsMisc_SRCS}" )

target_link_libraries ( CoffeeGraphicsMisc
        CoffeeCore
    )
