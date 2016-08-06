list (APPEND LIBS CoffeeGLeam )

if(COFFEE_BUILD_OCULUSVR)
    list ( APPEND LIBS CoffeeOVR )
endif()

coffee_add_application_long ( GLeamBaseTest "GLeam Basic" main.cpp "${LIBS}" "" "${COFFEE_ASSET_DIRECTORY}/eye-demo/" )
