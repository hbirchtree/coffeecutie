set ( AsioClientTest_SRCS
    main.cpp
    )

add_definitions("-DASIO_STANDALONE")

add_executable ( AsioClientTest ${AsioClientTest_SRCS} )

include_directories( ${COFFEE_CORE_INCLUDE_DIR} )

include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

target_link_libraries ( AsioClientTest
    CoffeeCore
    )

target_enable_cxx11(AsioClientTest)
