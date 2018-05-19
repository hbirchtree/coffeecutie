function ( GET_PERMISSION_FLAG TEST FLAG )
    set ( PERMISSIONS_LIST
        "NETWORK_ACCESS"
        "NETWORK_CONNECT"
        "NETWORK_SERVE"
        "WIFI_CONNECT"
        "WIFI_MANAGE"

        "SERIAL_PORTS"
        "BLUETOOTH"
        "NFC"
        "JOYSTICK"

        "AUDIO"

        "SIMPLE_GRAPHICS"
        "OPENGL"

        "CAMERA"
        "MICROPHONE"

        "ENVIRONMENT_SENSORS"
        "PROCESSES"

        "BROWSER"
        )
    foreach( PERM ${PERMISSIONS_LIST} )
        if( "${PERM}" MATCHES "${TEST}" )
            set( ${FLAG} "1" PARENT_SCOPE )
            return()
        endif()
    endforeach()
    set( ${FLAG} "0" PARENT_SCOPE)
endfunction()
