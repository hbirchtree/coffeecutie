# OpenSSL used for ASIO networking
find_package(OpenSSL REQUIRED)

set ( CoffeeASIO_SRCS
    private/rest-client.cpp
    private/asio_data.cpp
    )

include_directories( SYSTEM
    ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include
    ${OPENSSL_INCLUDE_DIR}
    )

coffee_add_library ( CoffeeASIO "${CoffeeASIO_SRCS}" "CoffeeCore;${OPENSSL_LIBRARIES}" )

