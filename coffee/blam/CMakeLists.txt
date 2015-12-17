set ( CBlamVolta_SRCS
        private/volta/cblam_bitm.cpp
        private/volta/cblam_map.cpp
        private/volta/cblam_mod2.cpp
        private/volta/cblam_scenario_loader.cpp
        )
set ( CBlamDimeter_SRCS
        private/dimeter/cblam_map.cpp
        )
set ( CBlamElegy_SRCS
        private/elegy/cblam_map.cpp
        )

include_directories(
        ${COFFEE_BLAM_INCLUDE_DIR}
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_CORE_INCLUDE_DIR}
    )

add_library ( CoffeeBlam ${COFFEE_LINK_OPT}
        ${CBlamVolta_SRCS}
        ${CBlamDimeter_SRCS}
        ${CBlamElegy_SRCS}
    )

target_link_libraries( CoffeeBlam
        CoffeeCore
        CoffeeImage
        CoffeeOpenGL
    )

set_property(TARGET CoffeeBlam PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeBlam PRIVATE cxx_variadic_templates)
