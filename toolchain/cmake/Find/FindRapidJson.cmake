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
    ${COFFEE_ROOT_DIR}

    PATH_SUFFIXES
    include/
    rapidjson/include/
    )

if(RAPIDJSON_INCLUDE_DIR_TMP)
    set ( RAPIDJSON_INCLUDE_DIR "${RAPIDJSON_INCLUDE_DIR_TMP}"
        CACHE PATH "RapidJSON include directory" )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(RapidJson REQUIRED_VARS RAPIDJSON_INCLUDE_DIR)
