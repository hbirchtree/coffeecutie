set(RAPIDJSON_INCLUDE_DIR)

find_path ( RAPIDJSON_INCLUDE_DIR
    NAMES rapidjson.h

    PATHS
    ${COFFEE_EXT_LIBRARY_DIR}/rapidjson
    ${RAPIDJSON_ROOT_DIR}

    PATH_SUFFIXES include include/rapidjson
    )

message ("FIX THIS FUCKING SHIT, GOD DAMN THIS CMAKE STUFF")
set(RAPIDJSON_INCLUDE_DIR ${COFFEE_EXT_LIBRARY_DIR}/rapidjson/include )

if(NOT RAPIDJSON_INCLUDE_DIR)
    message ( FATAL_ERROR "-- Failed to locate RapidJSON include directory" )
    set ( RAPIDJSON_FOUND NOTFOUND )
else()
    set ( RAPIDJSON_FOUND FOUND )
endif()

mark_as_advanced ( RAPIDJSON_INCLUDE_DIR )
