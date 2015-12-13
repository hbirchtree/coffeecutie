set ( CoffeeTestLib_SRCS
        private/rendering/cmultidrawgraph.cpp
        private/base_case.cpp
        private/cdrenderer.cpp
        )

set ( CoffeeTest_SRCS
        main.cpp
        )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_AUDIO_INCLUDE_DIR}
        ${COFFEE_ASSIMP_INCLUDE_DIR}
        ${COFFEE_BLAM_INCLUDE_DIR}
    )

add_library( CoffeeTestLib ${COFFEE_LINK_OPT} ${CoffeeTestLib_SRCS} )
add_executable ( CoffeeTest ${CoffeeTest_SRCS} )

target_link_libraries ( CoffeeTestLib
        CoffeeCore
        CBlam
        CAssimp
        CoffeeOpenGL
    )
target_link_libraries ( CoffeeTest
        CoffeeTestLib
    )

target_compile_features(CoffeeTestLib PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTest PRIVATE cxx_variadic_templates)
