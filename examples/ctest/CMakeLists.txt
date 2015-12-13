set ( CoffeeTestLib_SRCS
        rendering/cmultidrawgraph.cpp
        base_case.cpp
        cdrenderer.cpp
        )

set ( CoffeeTest_SRCS
        main.cpp
        )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/coffee/audio/include
        ${CMAKE_SOURCE_DIR}/coffee/blam/include
        ${CMAKE_SOURCE_DIR}/coffee/assimp/include
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/include
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/opengl/include
    )

add_library( CoffeeTestLib ${COFFEE_LINK_OPT} ${CoffeeTestLib_SRCS} )

add_executable ( CoffeeTest ${CoffeeTest_SRCS} )

target_link_libraries ( CoffeeTestLib
CoffeeCore
CBlam
CAudio
CAssimp
CoffeeOpenGL
)
target_link_libraries ( CoffeeTest
CoffeeTestLib
)

target_compile_features(CoffeeTestLib PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTest PRIVATE cxx_variadic_templates)
