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

include_directories(
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

target_link_libraries ( CoffeeGraphicsMisc
        CoffeeCore
    )
