coffee_application (
    TARGET EnvTest
    TITLE "Environment Data"
    SOURCES main.cpp
    LIBRARIES Core ComponentApp MediaFfmpeg
    )

if(TARGET ASIO)
    target_link_libraries ( EnvTest PUBLIC ASIO )
endif()
