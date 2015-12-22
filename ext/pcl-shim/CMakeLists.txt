set ( CPointClouds_SRCS
		
        )

include_directories(
        ${COFFEE_CORE_INCLUDE_DIR}
    )

add_library ( CoffeePCL ${COFFEE_LINK_OPT}
        ${CPointClouds_SRCS}
    )

target_link_libraries( CoffeePCL
        CoffeeCore
    )

set_property(TARGET CoffeePCL PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeePCL PRIVATE cxx_variadic_templates)
