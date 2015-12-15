include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

set ( CoffeeImage_SRCS
        private/cimage.cpp
        )

add_library( CoffeeImage ${COFFEE_LINK_OPT} ${CoffeeImage_SRCS})

target_link_libraries( CoffeeImage
        CoffeeCore
    )

set_property(TARGET CoffeeImage PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeImage PRIVATE cxx_variadic_templates)
