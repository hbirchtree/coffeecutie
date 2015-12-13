set ( CBlamTesting_SRCS
        main.cpp
        )

include_directories(
        ${COFFEE_BLAM_INCLUDE_DIR}
    )

add_executable ( CBlamTesting ${CBlamTesting_SRCS} )

target_compile_features(CBlamTesting PRIVATE cxx_variadic_templates)

target_link_libraries ( CBlamTesting
        CoffeeCore
        CBlam
    )
