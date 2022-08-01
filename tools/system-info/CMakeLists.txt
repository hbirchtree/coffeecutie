coffee_application (
    TARGET SystemInfo
    USE_CMD
    TITLE "System Info"
    SOURCES system_info.cpp
    LIBRARIES ComponentBundleSetup
    )

if(TARGET ASIO)
    target_link_libraries ( SystemInfo PUBLIC
        ASIO
        )
endif()

if(TARGET GLeamRHI)
    target_link_libraries ( SystemInfo PUBLIC
        GLeamRHI
        )
elseif(TARGET Gexxo)
    target_link_libraries ( SystemInfo PUBLIC
        Gexxo
        )
endif()
