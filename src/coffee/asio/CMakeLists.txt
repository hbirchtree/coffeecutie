option ( BUILD_OPENSSL "Build ASIO with OpenSSL support" ON )

# OpenSSL used for ASIO networking
if(BUILD_OPENSSL)
    dependency_get (
        REQUIRED
        SOURCE hbirchtree/native-library-bundle
        TAG ${NATIVE_LIB_BUNDLE_TAG}
        NAMES openssl=OpenSSL
        )

    set ( OPENSSL_ROOT_DIR "${OpenSSL_DIR}" )
    set ( ENV{OPENSSL_ROOT_DIR} "${OpenSSL_DIR}" )

    if(ANDROID OR (APPLE AND IOS))
        set ( OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.a" )
        set ( OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a" )
        set ( OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include" )
    endif()

    find_package ( OpenSSL REQUIRED )
endif()


if(TARGET OpenSSL::SSL)
    set ( LIBS OpenSSL::SSL )
    if(NOT WIN32 AND NOT MINGW AND NOT MSYS AND NOT EMSCRIPTEN)
        list ( APPEND LIBS dl )
    endif()
else()
    set ( LIBS $<BUILD_INTERFACE:${OPENSSL_LIBRARIES}> Threads::Threads )
endif()

find_package ( Threads QUIET )
if(TARGET Threads::Threads)
    set ( LIBS ${LIBS} Threads::Threads )
endif()

coffee_library (
    TARGET ASIO

    SOURCES
    private/asio_worker.cpp
    private/net_resource.cpp
    private/net_profiling.cpp

    LIBRARIES Core CoreState ${LIBS}

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_precompile_headers ( ASIO PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/http.h>
    $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/net_resource.h>
    $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/tcp_socket.h>
    $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/udp_socket.h>
    )

set ( ASIO_DIR "${COFFEE_EXT_LIBRARY_DIR}/asio" )

header_install (
    ASIO
    $<BUILD_INTERFACE:${ASIO_DIR}/asio/include>
    ""
    )

target_include_directories ( ASIO PUBLIC
    $<BUILD_INTERFACE:${ASIO_DIR}/asio/include>
    )

if(BUILD_OPENSSL)
    target_include_directories ( ASIO SYSTEM PUBLIC
        $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>
        )
    target_compile_definitions ( ASIO PUBLIC
        -DCOFFEE_ENABLE_SSL
        )

    dependency_link (
        TARGET ASIO
        DEPENDENCIES
            openssl=OpenSSL
        )
endif()

target_compile_definitions( ASIO PUBLIC
    -DASIO_STANDALONE
    )
