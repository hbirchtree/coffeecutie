find_library(OPENVR_LIBRARY NAMES "openvr_api" PATHS
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/lib/linux64
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/lib/osx32
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/lib/win64
    ${COFFEE_BINARY_DIR}/libs/lib
    ${OPENVR_LIBRARY_DIR}
    )

if(NOT DEFINED OPENVR_LIBRARY)
    message(FATAL_ERROR "Failed to locate OpenVR")
endif()

add_definitions(-DCOFFEE_OPENVR_ENABLED)

include_directories ( SYSTEM
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/headers
    ${OPENVR_INCLUDE_DIR}
    )

set ( CoffeeVR_SRCS
    private/copenvr.cpp
    )

coffee_add_library ( CoffeeVR "${CoffeeVR_SRCS}" )

target_link_libraries( CoffeeVR
    ${OPENVR_LIBRARY}
    CoffeeGLeam
    CoffeeCore
    )