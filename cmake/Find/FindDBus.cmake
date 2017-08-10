set(DBUS_SEARCH_PATHS
    /usr/local
    /usr
    ${RASPBERRY_SDK}/usr
    )

find_path ( DBUS_INCLUDE_DIR
    dbus-c++/dbus.h

    PATHS ${DBUS_SEARCH_PATHS}
    PATH_SUFFIXES include include/dbus-c++-1
    )

find_library ( DBUS_LIBRARY_TMP
    NAMES dbus-c++-1

    PATHS ${DBUS_SEARCH_PATHS}
    PATH_SUFFIXES lib "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
    )

if(DBUS_LIBRARY_TMP)
    set ( DBUS_LIBRARY "${DBUS_LIBRARY_TMP}" CACHE FILEPATH "" )
    set ( DBUS_INCLUDE_DIR "${DBUS_INCLUDE_DIR}" CACHE PATH "" )
    set ( DBUS_FOUND TRUE )
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(DBus
    REQUIRED_VARS
    DBUS_LIBRARY
    DBUS_INCLUDE_DIR
    )
