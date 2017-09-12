find_package ( DBus )

if(DBUS_FOUND)
    set ( DBUS_SRCS
        private/dbus-proc.cpp
        )

    coffee_add_library (
        CfDBusModule
        "${DBUS_SRCS}"
        "${DBUS_LIBRARY}"
        "${CMAKE_SOURCE_DIR}/include/coffee/misc/dbus"
        )
    target_include_directories( CfDBusModule
        PRIVATE
        $<BUILD_INTERFACE:${DBUS_INCLUDE_DIR}>
        )
endif()
