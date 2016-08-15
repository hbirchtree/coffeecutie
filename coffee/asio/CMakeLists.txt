set ( CoffeeASIO_SRCS
    private/rest-client.cpp
    private/asio_data.cpp
    )

include_directories( SYSTEM ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include )

coffee_add_library ( CoffeeASIO "${CoffeeASIO_SRCS}" )

# OpenSSL used for ASIO networking
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

target_link_libraries ( CoffeeASIO
    CoffeeCore
    ${OPENSSL_LIBRARIES}
    )
