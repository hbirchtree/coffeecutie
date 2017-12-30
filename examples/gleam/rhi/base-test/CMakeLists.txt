project ( GLeamBaseTest )
set ( GLeamBaseTest_SRCS
    main.cpp
    )

set ( USERDATA
    "${COFFEE_ASSET_DIRECTORY}/eye-demo/"
    )

set ( LIBS
    CoffeeGLeamRHI
    CoffeeRenderer_Default
    CoffeeCore
    )

coffee_add_application_long ( GLeamBaseTest_RHI "GLeam Basic RHI" "main.cpp;renderer.h" "${LIBS}" "" "${USERDATA}" USE_ASIO )
