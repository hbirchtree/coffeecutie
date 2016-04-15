set ( GLeamBaseTest_SRCS
    main.cpp
    )

list (APPEND LIBS CoffeeGLeam )

if(COFFEE_BUILD_OCULUSVR)
    list ( APPEND LIBS CoffeeOVR )
endif()

coffee_add_example ( GLeamBaseTest_RHI "GLeam Basic (RHI)" main.cpp "${LIBS}" )


