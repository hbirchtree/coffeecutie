find_package(PCL 1.7 REQUIRED COMPONENTS common io registration recognition keypoints modeler)

include_directories(
    ${COFFEE_EXT_FREENECT2_INCLUDE_DIR}
    ${PCL_INCLUDE_DIRS}
    )

coffee_add_example ( PCLTest main.cpp )
coffee_add_example ( CNectTest main2.cpp )

target_link_libraries ( PCLTest
    CoffeeCore
    CoffeeImage
    CoffeePCL
    CoffeeNect
    )

target_link_libraries ( CNectTest
    CoffeeCore
    CoffeeImage
    CoffeeNect
    )

target_enable_cxx11(PCLTest)
target_enable_cxx11(CNectTest)
