include_directories( SYSTEM
    ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include
    )

if(NOT WIN32 AND NOT ANDROID)
    include_directories( ${OPENSSL_INCLUDE_DIR})
endif()

coffee_add_application ( AsioClientTest "ASIO Test" main.cpp CoffeeASIO )
