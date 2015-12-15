add_subdirectory(opengl)

set ( CoffeeGraphicsMisc_SRCS
        private/scene/cnode.cpp
    )

add_library( CoffeeGraphicsMisc ${COFFEE_LINK_OPT} ${CoffeeGraphicsMisc_SRCS} )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries ( CoffeeGraphicsMisc
        CoffeeCore
    )

set_property(TARGET CoffeeGraphicsMisc PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeGraphicsMisc PRIVATE cxx_variadic_templates)
