find_path ( RAPIDJSON_INCLUDE_DIR
    rapidjson.h

    PATHS
    ${COFFEE_EXT_LIBRARY_DIR}/rapidjson/include
    ${RAPIDJSON_ROOT_DIR}

    PATH_PREFIXES
    rapidjson/

    NO_DEFAULT_PATH
    )

message ( "FIX THIS SHIT" )

set ( RAPIDJSON_INCLUDE_DIR "${COFFEE_EXT_LIBRARY_DIR}/rapidjson/include" )

if(NOT RAPIDJSON_INCLUDE_DIR)
    message ( FATAL_ERROR "-- Failed to locate RapidJSON include directory" )
    set ( RAPIDJSON_INCLUDE_DIR NOTFOUND )
endif()

set ( RAPIDJSON_FOUND FOUND )

mark_as_advanced ( RAPIDJSON_INCLUDE_DIR )
