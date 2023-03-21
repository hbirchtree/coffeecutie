coffee_application (
    TARGET GLeamBaseTest_RHI
    TITLE "GLeam Basic RHI"

    SOURCES
    main.cpp
    renderer.h

    LIBRARIES
    GLeamRHI
    DropinRenderer
    Core
    EntComp

    RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/basic_test
    )

set_target_properties ( GLeamBaseTest_RHI PROPERTIES
    EMSCRIPTEN_MAX_HEAP_SIZE 20M
    )

if(TARGET GpuQuery)
    target_link_libraries ( GLeamBaseTest_RHI PUBLIC GpuQuery )
endif()

if(TARGET ASIO)
    target_link_libraries ( GLeamBaseTest_RHI PUBLIC ASIO )
    if(TARGET DiscordLatte)
        target_link_libraries ( GLeamBaseTest_RHI PUBLIC DiscordLatte )
    endif()
endif()
