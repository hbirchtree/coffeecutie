find_package(PCL 1.7 REQUIRED COMPONENTS common io registration recognition keypoints modeler)

include_directories(
    ${COFFEE_EXT_FREENECT2_INCLUDE_DIR}
    ${PCL_INCLUDE_DIRS}
    )

add_executable ( PCLTest main.cpp )
add_executable ( CNectTest main2.cpp )

target_link_libraries ( PCLTest
    CoffeeCore
    CoffeeImage
    CoffeeGraphicsMisc
    CoffeePCL
    )

target_link_libraries ( CNectTest
    CoffeeCore
    CoffeeImage
    CoffeeGraphicsMisc
    CoffeeNect
    )

target_enable_cxx11(PCLTest)
target_enable_cxx11(CNectTest)
