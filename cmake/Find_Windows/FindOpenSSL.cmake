set ( OPENSSL_SEARCH_PATHS
    "C:\\OpenSSL-Win64"
    "C:\\OpenSSL-Win32"
    )

find_library ( SSL_CRYPTO_LIBRARY_TMP
    crypto

    PATH_SUFFIXES
    lib
    PATHS
    ${OPENSSL_SEARCH_PATHS}
    )
find_library ( SSL_LIBRARY_TMP
    ssl

    PATH_SUFFIXES
    lib
    PATHS
    ${OPENSSL_SEARCH_PATHS}
    )

find_path ( SSL_INCLUDE_DIR_TMP
    openssl/ssl.h
    openssl/ssl2.h
    openssl/ssl3.h

    PATH_SUFFIXES
    include
    PATHS
    ${OPENSSL_SEARCH_PATHS}
    )

#set ( OPENSSL_LIBRARIES "${SSL_LIBRARY_TMP};${SSL_CRYPTO_LIBRARY_TMP}" CACHE STRING "" )
set ( OPENSSL_LIBRARIES "C:/OpenSSL-Win64/lib/libssl.lib;C:/OpenSSL-Win64/lib/libcrypto.lib" CACHE STRING "" )
set ( OPENSSL_INCLUDE_DIR "${SSL_INCLUDE_DIR_TMP}" CACHE PATH "" )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenSSL
    REQUIRED_VARS
    OPENSSL_LIBRARIES
    OPENSSL_INCLUDE_DIR
    )
