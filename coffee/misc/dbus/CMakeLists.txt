find_package ( DBus )

if(DBUS_FOUND)
    set ( DBUS_SRCS
        private/dbus-proc.cpp
        )

    include_directories(
        ${DBUS_INCLUDE_DIR}
        )

    coffee_add_library (
        CfDBusModule
        "${DBUS_SRCS}"
        "CoffeeCore;${DBUS_LIBRARY}"
        "${CMAKE_SOURCE_DIR}/include/coffee/misc/dbus"
        )
endif()
