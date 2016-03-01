set ( IRC_Client_SRCS
    main.cpp
    )

add_definitions("-DASIO_STANDALONE")

include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

coffee_add_example ( IRC_Client ${IRC_Client_SRCS} )

target_link_libraries ( IRC_Client
    CoffeeCore
    )
