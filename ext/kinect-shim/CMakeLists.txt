message("-- Configuring Freenect2 extension lib")

set ( CNect_SRCS
    private/cnect.cpp
    )

include_directories(
    ${COFFEE_EXT_FREENECT2_INCLUDE_DIR}
    ${FREENECT2_INCLUDE_DIR}
    )

find_library(FREENECT2_LIBRARY NAMES "freenect2" PATHS ${CMAKE_CURRENT_BINARY_DIR}/libs/lib)

if(NOT FREENECT2_FOUND)
    message(error "-- Failed to configure Freenect2 extension, no Freenect2 library found!")
endif()

coffee_add_library ( CoffeeNect
    ${CNect_SRCS}
    )

target_link_libraries( CoffeeNect
    CoffeeCore
    ${FREENECT2_LIBRARY}
    )

set_property(TARGET CoffeeNect PROPERTY POSITION_INDEPENDENT_CODE ON)
target_enable_cxx11(CoffeeNect)