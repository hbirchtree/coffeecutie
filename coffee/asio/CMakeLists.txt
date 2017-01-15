# OpenSSL used for ASIO networking
if(NOT WIN32 AND NOT ANDROID)
    find_package(OpenSSL REQUIRED)
endif()

set ( CoffeeASIO_SRCS
    private/rest-client.cpp
    private/asio_data.cpp
    private/tcp-socket.cpp
    private/udp-socket.cpp
    )
set ( LIBS "CoffeeCore" )

include_directories( SYSTEM
    ${COFFEE_EXT_LIBRARY_DIR}/asio/asio/include
    )
if(NOT WIN32 AND NOT ANDROID)
    include_directories( SYSTEM
        ${OPENSSL_INCLUDE_DIR}
        )
    set ( LIBS "${LIBS};${OPENSSL_LIBRARIES}" )
endif()


coffee_add_library ( CoffeeASIO "${CoffeeASIO_SRCS}" "${LIBS}"
    "${CMAKE_SOURCE_DIR}/include/coffee/asio")

