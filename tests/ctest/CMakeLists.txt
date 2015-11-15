set ( CoffeeTestLib_SRCS
        tests/ctest/rendering/cmultidrawgraph.cpp
        tests/ctest/base_case.cpp
        tests/ctest/cdrenderer.cpp
        )

set ( CoffeeTest_SRCS
        tests/ctest/main.cpp
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
