
set ( GLeamBaseTest_SRCS
    main.cpp
    )

list (APPEND USERDATA
    "${COFFEE_ASSET_DIRECTORY}/eye-demo/"
    )

list (APPEND LIBS CoffeeGLeam )

if(COFFEE_BUILD_OCULUSVR)
    list ( APPEND LIBS CoffeeOVR )
endif()

coffee_add_application_long ( GLeamBaseTest_RHI "GLeam Basic RHI" main.cpp "${LIBS}" "" "${USERDATA}" )
