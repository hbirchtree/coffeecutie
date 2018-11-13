option ( BUILD_OPENSSL "Build ASIO with OpenSSL support" ON )

# OpenSSL used for ASIO networking
if(BUILD_OPENSSL)
    find_package(OpenSSL REQUIRED)
endif()

set ( CoffeeASIO_SRCS
    )

if(TARGET OpenSSL::SSL)
    set ( LIBS OpenSSL::SSL )
else()
    set ( LIBS $<BUILD_INTERFACE:${OPENSSL_LIBRARIES}> )
endif()

coffee_library (
    TARGET CoffeeASIO

    SOURCES
    private/asio_worker.cpp
    private/net_resource.cpp
    private/net_profiling.cpp

    LIBRARIES CoffeeCore ${LIBS}

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

set ( ASIO_DIR "${COFFEE_EXT_LIBRARY_DIR}/asio" )

target_include_directories ( CoffeeASIO SYSTEM PUBLIC
    $<BUILD_INTERFACE:${ASIO_DIR}/asio/include>
    $<INSTALL_INTERFACE:include>
    )

if(BUILD_OPENSSL)
    target_include_directories ( CoffeeASIO SYSTEM PUBLIC
        $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>
        )
    target_compile_definitions ( CoffeeASIO PUBLIC
        -DCOFFEE_ENABLE_SSL
        )
endif()

target_compile_definitions( CoffeeASIO PUBLIC
    -DASIO_STANDALONE
    )

coffee_bundle_includes ( "${ASIO_DIR}/asio/include" )
