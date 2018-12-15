find_path ( STB_INCLUDE_DIR
    NAMES
    stb.h

    PATHS
    /usr
    /usr/local
    /Library/Frameworks
    ~/Library/Frameworks
    ${COFFEE_EXT_LIBRARY_DIR}/stb
    ${CMAKE_SOURCE_DIR}/libs
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    stb

    NO_CMAKE_FIND_ROOT_PATH
    )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(stb REQUIRED_VARS STB_INCLUDE_DIR)
