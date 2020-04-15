set ( OPENSSL_SEARCH_PATHS
    ${OpenSSL_DIR}
    ${OPENSSL_ROOT_DIR}
    )

find_library ( SSL_CRYPTO_LIBRARY_TMP
    libcrypto

    PATH_SUFFIXES
    lib

    PATHS
    ${OPENSSL_SEARCH_PATHS}
    )
find_library ( SSL_LIBRARY_TMP
    libssl

    PATH_SUFFIXES
    lib
    
    PATHS
    ${OPENSSL_SEARCH_PATHS}
    )

find_path ( SSL_INCLUDE_DIR_TMP
    ssl.h
    ssl2.h
    ssl3.h

    PATH_SUFFIXES
    include
    include/openssl
    
    PATHS
    ${OPENSSL_SEARCH_PATHS}
    )

if(NOT TARGET OpenSSL::SSL)
    add_library ( OpenSSL::SSL STATIC IMPORTED )
endif()

set_target_properties ( OpenSSL::SSL PROPERTIES
    IMPORTED_LOCATION "${SSL_LIBRARY_TMP}"
    INTERFACE_LINK_LIBRARIES "${SSL_CRYPTO_LIBRARY_TMP}"
    INTERFACE_INCLUDE_DIRECTORIES "${SSL_INCLUDE_DIR_TMP}"
    )

if(SSL_LIBRARY_TMP AND SSL_CRYPTO_LIBRARY_TMP AND SSL_INCLUDE_DIR_TMP)
    set ( OpenSSL_FOUND TRUE )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenSSL
    REQUIRED_VARS
    SSL_LIBRARY_TMP
    SSL_CRYPTO_LIBRARY_TMP
    SSL_INCLUDE_DIR_TMP
    OpenSSL_FOUND
    )
