include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

coffee_add_example ( IRC_Client "IRC client" main.cpp CoffeeCore )