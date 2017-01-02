include_directories( SYSTEM
    ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include
    ${OPENSSL_INCLUDE_DIR}
    )

coffee_add_application ( AsioClientTest "ASIO Test" main.cpp CoffeeASIO )
