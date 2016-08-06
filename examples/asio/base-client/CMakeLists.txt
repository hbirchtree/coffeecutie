include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

coffee_add_application ( AsioClientTest "ASIO Test" main.cpp CoffeeASIO )
