coffee_application (
    TARGET SystemInfo
    USE_CMD
    TITLE "System Info"
    SOURCES system_info.cpp
    LIBRARIES ComponentBundleSetup
    )

if(TARGET GLeamRHI_ES)
    target_link_libraries ( SystemInfo PUBLIC
        GLeamRHI_ES
        )
elseif(TARGET Gexxo)
    target_link_libraries ( SystemInfo PUBLIC
        Gexxo
        )
endif()
