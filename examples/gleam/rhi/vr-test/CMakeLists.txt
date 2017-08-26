set ( USERDATA
    "${COFFEE_ASSET_DIRECTORY}/eye-demo/"
    )

set ( BULLET_ROOT "${NATIVE_LIBRARY_DIR}" )
#find_package(Bullet QUIET)

find_package(OpenVR QUIET)
include_directories(${OPENVR_INCLUDE_DIR} ${BULLET_INCLUDE_DIRS})

set ( LIBS
    CoffeeRenderer_Default
    ${OPENVR_LIBRARY}
    ${BULLET_LIBRARIES}
    CoffeeCore
    )

coffee_add_application_long ( VRBaseTest_RHI "VR Basic RHI" main.cpp
    "${LIBS}" "" "${USERDATA}" )
