if(COFFEE_BUILD_PCL)

set ( PCLTest_SRCS
        main.cpp
        )

find_package(PCL 1.7 REQUIRED COMPONENTS common io registration recognition keypoints modeler)

include_directories(
        ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
        ${COFFEE_GRAPHICS_INCLUDE_DIR}
        ${PCL_INCLUDE_DIRS}
    )

add_executable ( PCLTest ${PCLTest_SRCS} )

target_link_libraries ( PCLTest
        CoffeeCore
        CoffeeImage
        CoffeeGraphicsMisc
        CoffeePCL
        CoffeeNect
    )

target_compile_features(PCLTest PRIVATE cxx_variadic_templates)

endif()
