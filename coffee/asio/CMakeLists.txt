set ( CoffeeASIO_SRCS
    private/rest-client.cpp
    private/asio_data.cpp
    )

include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

coffee_add_library ( CoffeeASIO "${CoffeeASIO_SRCS}" )
target_link_libraries ( CoffeeASIO
    CoffeeCore
    )
