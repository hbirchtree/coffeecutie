set ( CBlamTesting_SRCS
        ${COFFEE_EXAMPLE_DIR}/cblam-testing/main.cpp
        )

add_executable ( CBlamTesting ${CBlamTesting_SRCS} )

target_compile_features(CBlamTesting PRIVATE cxx_variadic_templates)

target_link_libraries ( CBlamTesting
CoffeeCore
CBlam
)
