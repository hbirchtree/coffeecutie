# OpenSSL used for ASIO networking
if(COFFEE_BUILD_OPENSSL)
    find_package(OpenSSL REQUIRED)
endif()

set ( CoffeeASIO_SRCS
    private/rest-client.cpp
    private/asio_data.cpp
    private/tcp-socket.cpp
    private/udp-socket.cpp
    )

set ( LIBS ${OPENSSL_LIBRARIES} )

coffee_add_library ( CoffeeASIO "${CoffeeASIO_SRCS}" "${LIBS}"
    "${CMAKE_SOURCE_DIR}/include/coffee/asio")

target_include_directories ( CoffeeASIO
    PUBLIC
    ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include
    ${OPENSSL_INCLUDE_DIR}
    )
