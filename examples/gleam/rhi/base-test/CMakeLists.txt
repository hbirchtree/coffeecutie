coffee_application (
    TARGET GLeamBaseTest_RHI
    TITLE "GLeam Basic RHI"

    SOURCES
    main.cpp
    renderer.h

    LIBRARIES
    GLeamRHI
    RendererDefault
    Core
    EntComp

    RESOURCES
    "${COFFEE_ASSET_DIRECTORY}/eye-demo/"
    )


#if(TARGET ASIO)
    target_link_libraries ( GLeamBaseTest_RHI PUBLIC ASIO )
#endif()

if(TARGET DiscordLatte)
    target_link_libraries ( GLeamBaseTest_RHI PUBLIC DiscordLatte )
endif()
