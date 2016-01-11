if(NOT DEFINED FREENECT2_INCLUDE_DIR)
    message("-- Freenect2 include directory not found, will not build CoffeeNect")
else()

set ( CNect_SRCS
        private/cnect.cpp
    )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COFFEE_CORE_INCLUDE_DIR}
		${FREENECT2_INCLUDE_DIR}
    )

link_directories(
        ${PCL_LIBRARY_DIRS}
    )

find_library(FREENECT2_LIBRARY NAMES "freenect2" PATHS ${CMAKE_CURRENT_BINARY_DIR}/libs/lib)

add_library ( CoffeeNect ${COFFEE_LINK_OPT}
        ${CNect_SRCS}
    )

target_link_libraries( CoffeeNect
        CoffeeCore
        ${FREENECT2_LIBRARY}
    )

set_property(TARGET CoffeeNect PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeNect PRIVATE cxx_variadic_templates)

endif()