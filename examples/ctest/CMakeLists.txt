set ( CoffeeTestLib_SRCS
        ${COFFEE_EXAMPLE_DIR}/ctest/rendering/cmultidrawgraph.cpp
        ${COFFEE_EXAMPLE_DIR}/ctest/base_case.cpp
        ${COFFEE_EXAMPLE_DIR}/ctest/cdrenderer.cpp
        )

set ( CoffeeTest_SRCS
        ${COFFEE_EXAMPLE_DIR}/ctest/main.cpp
        )

add_library( CoffeeTestLib ${COFFEE_LINK_OPT} ${CoffeeTestLib_SRCS} )

add_executable ( CoffeeTest ${CoffeeTest_SRCS} )

target_link_libraries ( CoffeeTestLib
CoffeeCore
CBlam
CAudio
CAssimp
)
target_link_libraries ( CoffeeTest
CoffeeTestLib
)

target_compile_features(CoffeeTestLib PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTest PRIVATE cxx_variadic_templates)
