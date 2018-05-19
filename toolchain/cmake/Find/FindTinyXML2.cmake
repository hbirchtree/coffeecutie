find_path ( TINYXML2_INCLUDE_DIR
    NAMES
    tinyxml2.h

    PATHS
    /usr
    /usr/local
    /Library/Frameworks
    ~/Library/Frameworks
    ${COFFEE_EXT_LIBRARY_DIR}/tinyxml2
    ${CMAKE_SOURCE_DIR}/libs
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    tinyxml2
    )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(TinyXML2 REQUIRED_VARS TINYXML2_INCLUDE_DIR)
