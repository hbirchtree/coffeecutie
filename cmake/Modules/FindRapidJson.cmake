find_path ( RAPIDJSON_INCLUDE_DIR
    rapidjson/rapidjson.h

    PATHS
    ${COFFEE_EXT_LIBRARY_DIR}/rapidjson
    ${RAPIDJSON_ROOT_DIR}

    PATH_SUFFIXES
    include
    )

if(NOT RAPIDJSON_INCLUDE_DIR)
    message ( FATAL_ERROR "-- Failed to locate RapidJSON include directory" )
    set ( RAPIDJSON_INCLUDE_DIR NOTFOUND )
endif()

set ( RAPIDJSON_FOUND FOUND )

mark_as_advanced ( RAPIDJSON_INCLUDE_DIR )