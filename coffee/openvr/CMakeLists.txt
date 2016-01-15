find_library(OPENVR_LIBRARY NAMES "openvr_api" PATHS
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/lib/linux64
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/lib/osx32
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/lib/win64
    ${COFFEE_BINARY_DIR}/libs/lib
    ${OPENVR_LIBRARY_DIR}
    )

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${COFFEE_GRAPHICS_INCLUDE_DIR}
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/headers
    ${OPENVR_INCLUDE_DIR}
    )

set ( CoffeeVR_SRCS
    private/copenvr.cpp
    )

add_library( CoffeeVR ${COFFEE_LINK_OPT} ${CoffeeVR_SRCS})

target_link_libraries( CoffeeVR
    ${OPENVR_LIBRARY}
    CoffeeGLeam
    CoffeeCore
    )

set_property(TARGET CoffeeVR PROPERTY POSITION_INDEPENDENT_CODE ON)
target_enable_cxx11(CoffeeVR)