set ( CoffeeTestHud_SRCS
        main.cpp
        )

include_directories(
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/include
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/opengl/include
    )

add_executable ( CoffeeTestHud ${CoffeeTestHud_SRCS} )

target_link_libraries ( CoffeeTestHud
CoffeeCore
CoffeeGraphicsMisc
CoffeeOpenGL
)

target_compile_features(CoffeeTestHud PRIVATE cxx_variadic_templates)
