set ( CBlamVolta_SRCS
        volta/cblam_bitm.cpp
        volta/cblam_map.cpp
        volta/cblam_mod2.cpp
        volta/cblam_scenario_loader.cpp
        )
set ( CBlamDimeter_SRCS
        dimeter/cblam_map.cpp
        )
set ( CBlamElegy_SRCS
        elegy/cblam_map.cpp
        )

include_directories(
        ${COFFEE_IMAGE_INCLUDE_DIR}
        ${COFFEE_CORE_INCLUDE_DIR}
    )

add_library ( CBlam ${COFFEE_LINK_OPT}
        ${CBlamVolta_SRCS}
        ${CBlamDimeter_SRCS}
        ${CBlamElegy_SRCS}
    )

target_link_libraries( CBlam
        CoffeeCore
        CImage
    )

target_compile_features(CBlam PRIVATE cxx_variadic_templates)
