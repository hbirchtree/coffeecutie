find_path ( RAPIDJSON_INCLUDE_DIR_TMP
    NAMES
    rapidjson/rapidjson.h

    PATHS
    /usr
    /usr/local
    /Library/Frameworks
    ~/Library/Frameworks
    ${COFFEE_EXT_LIBRARY_DIR}/rapidjson
    ${RAPIDJSON_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/libs

    PATH_SUFFIXES
    include/
    rapidjson/include/
    )

set ( RAPIDJSON_INCLUDE_DIR "${RAPIDJSON_INCLUDE_DIR_TMP}" CACHE PATH "RapidJSON include directory" )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(RapidJson REQUIRED_VARS RAPIDJSON_INCLUDE_DIR)
