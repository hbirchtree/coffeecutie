project ( GLeamBaseTest )
set ( GLeamBaseTest_SRCS
    main.cpp
    )

list (APPEND USERDATA
    "${COFFEE_ASSET_DIRECTORY}/eye-demo/"
    )

find_package(OpenVR QUIET)
include_directories(${OPENVR_INCLUDE_DIR})

list (APPEND LIBS CoffeeGLeam ${OPENVR_LIBRARY} )

coffee_add_application_long ( VRBaseTest_RHI "VR Basic RHI" main.cpp "${LIBS}" "" "${USERDATA}" )
