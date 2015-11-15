set ( COFFEE_CBLAM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/coffee/blam )

set ( CBlamVolta_SRCS
        ${COFFEE_CBLAM_SRC_DIR}/volta/cblam_bitm.cpp
        ${COFFEE_CBLAM_SRC_DIR}/volta/cblam_map.cpp
        ${COFFEE_CBLAM_SRC_DIR}/volta/cblam_mod2.cpp
        ${COFFEE_CBLAM_SRC_DIR}/volta/cblam_scenario_loader.cpp
        )
set ( CBlamDimeter_SRCS
        ${COFFEE_CBLAM_SRC_DIR}/dimeter/cblam_map.cpp
        )
set ( CBlamElegy_SRCS
        ${COFFEE_CBLAM_SRC_DIR}/elegy/cblam_map.cpp
        )

add_library ( CBlam ${COFFEE_LINK_OPT}
            ${CBlamVolta_SRCS}
            ${CBlamDimeter_SRCS}
            ${CBlamElegy_SRCS}
            )

target_compile_features(CBlam PRIVATE cxx_variadic_templates)
