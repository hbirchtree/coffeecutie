coffee_application (
    TARGET CompAppTest
    TITLE "Component app"
    SOURCES main.cpp
    LIBRARIES ComponentBundleSetup GLeamRHI_ES
    )

if(TARGET GLeamRHI)
    target_link_libraries ( CompAppTest PUBLIC
        GLeamRHI
        )
endif()

if(TARGET Gexxo)
    target_link_libraries ( CompAppTest PUBLIC
        Gexxo
        )
endif()
