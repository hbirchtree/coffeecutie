find_package(PCL QUIET 1.7 COMPONENTS common io registration recognition surface keypoints modeler visualization)

if(NOT DEFINED PCL_FOUND)
    message("-- PCL not found, not building pcl-shim!")
else()
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
    include_directories( SYSTEM
        ${PCL_INCLUDE_DIRS}
        )
    link_directories(
        ${PCL_LIBRARY_DIRS}
        )
    add_definitions(${PCL_DEFINITIONS})

    set ( CPointClouds_SRCS
        private/cpcl.cpp
        )
    coffee_add_library ( CoffeePCL
        ${CPointClouds_SRCS}
        )
    target_link_libraries( CoffeePCL
        CoffeeCore
        ${PCL_COMMON_LIBRARIES}
        ${PCL_IO_LIBRARIES}
        ${PCL_REGISTRATION_LIBRARIES}
        ${PCL_SURFACE_LIBRARIES}
        ${PCL_RECOGNITION_LIBRARIES}
        ${PCL_KEYPOINTS_LIBRARIES}
        ${PCL_MODELER_LIBRARIES}
        ${PCL_VISUALIZATION_LIBRARIES}
        )
    set_property(TARGET CoffeePCL PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_compile_features(CoffeePCL PRIVATE cxx_variadic_templates)
endif() #PCL_FOUND
