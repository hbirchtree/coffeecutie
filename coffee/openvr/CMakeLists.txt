find_package(OpenVR)

add_definitions(-DCOFFEE_OPENVR_ENABLED)

include_directories ( SYSTEM
    ${COFFEE_EXT_LIBRARY_DIR}/openvr/headers
    ${OPENVR_INCLUDE_DIR}
    )

set ( CoffeeVR_SRCS
    private/copenvr.cpp
    private/ovrdevice.cpp
    private/ovractor.cpp
    private/ovr_private.cpp
    )

coffee_add_library ( CoffeeVR "${CoffeeVR_SRCS}" )

target_link_libraries( CoffeeVR
    ${OPENVR_LIBRARY}
    CoffeeGLeam
    CoffeeCore
    )
