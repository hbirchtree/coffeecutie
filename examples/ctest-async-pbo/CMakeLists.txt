set ( AsyncPbo_SRCS
        main.cpp
        )

include_directories(
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/include
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/opengl/include
    )

add_executable ( AsyncPbo ${AsyncPbo_SRCS} )

target_link_libraries ( AsyncPbo
CoffeeCore
CoffeeGraphicsMisc
CoffeeOpenGL
)

target_compile_features(AsyncPbo PRIVATE cxx_variadic_templates)
