find_path ( RAPIDJSON_INCLUDE_DIR_TMP
    NAMES
    rapidjson.h

    PATHS
    /usr
    /usr/local
    /Library/Frameworks
    ~/Library/Frameworks
    ${COFFEE_EXT_LIBRARY_DIR}/rapidjson
    ${RAPIDJSON_ROOT_DIR}

    PATH_SUFFIXES
    include
    include/rapidjson
    )

set ( RAPIDJSON_INCLUDE_DIR "${RAPIDJSON_INCLUDE_DIR_TMP}" CACHE PATH "RapidJSON include directory" )

set ( RAPIDJSON_INCLUDE_DIR ${COFFEE_EXT_LIBRARY_DIR}/rapidjson/include )

mark_as_advanced ( RAPIDJSON_INCLUDE_DIR )
