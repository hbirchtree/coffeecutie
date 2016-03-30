set ( CoffeeTestLib_SRCS
    private/rendering/cmultidrawgraph.cpp
    private/base_case.cpp
    private/cdrenderer.cpp
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

coffee_add_elibrary ( CoffeeTestLib ${COFFEE_LINK_OPT} "${CoffeeTestLib_SRCS}" )
coffee_add_example ( CoffeeTest "Botch Renderer 1" main.cpp CoffeeTestLib )

target_link_libraries ( CoffeeTestLib
    CoffeeCore
    CoffeeBlam
    CoffeeAssimp
    CoffeeOpenGL
    )
