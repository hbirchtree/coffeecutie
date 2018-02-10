# OpenSSL used for ASIO networking
if(COFFEE_BUILD_OPENSSL)
    find_package(OpenSSL REQUIRED)
endif()

set ( CoffeeASIO_SRCS
    private/rest-client.cpp
    private/net_resource.cpp
    private/net_profiling.cpp
    )

if(TARGET OpenSSL::SSL)
    set ( LIBS OpenSSL::SSL )
else()
    set ( LIBS $<BUILD_INTERFACE:${OPENSSL_LIBRARIES}> )
endif()

coffee_add_library ( CoffeeASIO "${CoffeeASIO_SRCS}" "${LIBS}"
    "${CMAKE_SOURCE_DIR}/include/coffee/asio")

set ( ASIO_DIR "${COFFEE_EXT_LIBRARY_DIR}/asio" )

target_include_directories ( CoffeeASIO SYSTEM
    PUBLIC
    $<BUILD_INTERFACE:${ASIO_DIR}/asio/include>

    $<INSTALL_INTERFACE:include/>
    )

if(COFFEE_BUILD_OPENSSL)
    target_include_directories ( CoffeeASIO SYSTEM
        PUBLIC
        $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>
        )
endif()

coffee_bundle_includes ( "${ASIO_DIR}/asio/include" )
