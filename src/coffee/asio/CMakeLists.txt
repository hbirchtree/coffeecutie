option ( BUILD_OPENSSL "Build ASIO with OpenSSL support" ON )

# OpenSSL used for ASIO networking
if(BUILD_OPENSSL)
    dependency_get (
        REQUIRED
        SOURCE hbirchtree/native-library-bundle
        TAG ${NATIVE_LIB_BUNDLE_TAG}
        NAMES openssl=OpenSSL
        )

    set ( OPENSSL_ROOT_DIR "${GIT_DEP_DIR}/openssl" )
    set ( ENV{OPENSSL_ROOT_DIR} "${GIT_DEP_DIR}/openssl" )

    if(ANDROID OR (APPLE AND IOS))
        set ( OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.a" )
        set ( OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a" )
        set ( OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include" )
    endif()

    find_package ( OpenSSL REQUIRED )
endif()


if(TARGET OpenSSL::SSL)
    set ( LIBS OpenSSL::SSL )
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
    $<INSTALL_INTERFACE:include>
    )

set ( ASIO_DIR "${COFFEE_EXT_LIBRARY_DIR}/asio" )

target_include_directories ( ASIO SYSTEM PUBLIC
    $<BUILD_INTERFACE:${ASIO_DIR}/asio/include>
    $<INSTALL_INTERFACE:include>
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

coffee_bundle (
    DESTINATION libs

    HEADER_DIRECTORIES
    "${ASIO_DIR}/asio/include"
    )
