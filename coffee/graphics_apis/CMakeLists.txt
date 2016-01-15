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

add_library( CoffeeGraphicsMisc ${COFFEE_LINK_OPT} ${CoffeeGraphicsMisc_SRCS} )

include_directories(
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
    )

target_link_libraries ( CoffeeGraphicsMisc
        CoffeeCore
    )

set_property(TARGET CoffeeGraphicsMisc PROPERTY POSITION_INDEPENDENT_CODE ON)
target_enable_cxx11(CoffeeGraphicsMisc)
