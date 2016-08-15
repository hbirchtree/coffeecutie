include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

coffee_add_application ( IRC_Client "IRC client" main.cpp CoffeeASIO )
