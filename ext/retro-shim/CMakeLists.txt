set ( CoffeeRetro_SRCS
        )

coffee_add_library( CoffeeRetro ${COFFEE_LINK_OPT} ${CoffeeRetro_SRCS})

target_link_libraries( CoffeeRetro
                CoffeeGLeam
                CoffeeCore
    )
