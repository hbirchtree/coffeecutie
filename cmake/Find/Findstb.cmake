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

    PATH_SUFFIXES
    stb
    )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(stb REQUIRED_VARS STB_INCLUDE_DIR)