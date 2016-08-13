find_path ( NATIVE_LIBRARY_DIR_TMP
    NAMES
    README.md
    PATHS
    ${CMAKE_SOURCE_DIR}/../native-library-bundle/
    )

set ( NATIVE_LIBRARY_DIR ${NATIVE_LIBRARY_DIR_TMP} CACHE PATH "" )
