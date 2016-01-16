set ( AsioClientTest_SRCS
    main.cpp
    )

add_definitions("-DASIO_STANDALONE")

include_directories( ${COFFEE_CORE_INCLUDE_DIR} )

include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

coffee_add_example ( AsioClientTest ${AsioClientTest_SRCS} )

target_link_libraries ( AsioClientTest
    CoffeeCore
    )