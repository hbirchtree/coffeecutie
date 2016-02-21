set ( CBlam_SRCS
        private/volta/cblam_bitm.cpp
        private/volta/cblam_map.cpp
        private/volta/cblam_mod2.cpp
        private/volta/cblam_scenario_loader.cpp
        )

coffee_add_library ( CoffeeBlam "${CBlam_SRCS}" )

target_link_libraries( CoffeeBlam
        CoffeeCore
        CoffeeImage
    )
