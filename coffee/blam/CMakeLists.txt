set ( CBlam_SRCS
        private/volta/cblam_bitm.cpp
        private/volta/cblam_map.cpp
        private/volta/cblam_mod2.cpp
        private/volta/cblam_scenario_loader.cpp

        private/dimeter/cblam_map.cpp

        private/elegy/cblam_map.cpp
        )

include_directories(
        ${COFFEE_BLAM_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_CORE_INCLUDE_DIR}
    )

coffee_add_library ( CoffeeBlam "${CBlam_SRCS}" )

target_link_libraries( CoffeeBlam
        CoffeeCore
        CoffeeImage
    )
