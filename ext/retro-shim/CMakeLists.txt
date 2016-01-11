set ( CoffeeRetro_SRCS
        )

add_library( CoffeeRetro ${COFFEE_LINK_OPT} ${CoffeeRetro_SRCS})

target_link_libraries( CoffeeRetro
                CoffeeGLeam
                CoffeeCore
    )

set_property(TARGET CoffeeRetro PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeRetro PRIVATE cxx_variadic_templates)
