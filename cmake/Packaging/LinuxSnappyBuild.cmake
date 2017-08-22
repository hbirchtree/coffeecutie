if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    include ( PermissionList )

    set ( SNAPPY_CONFIG_DIR "${COFFEE_DESKTOP_DIRECTORY}/linux/snappy" CACHE PATH "" )

    set ( SNAPPY_PROGRAM "/usr/bin/snapcraft" CACHE FILEPATH "" )

    set ( SNAPPY_PACKAGING_CAT "linux-snappy" CACHE STRING "" )

    set ( SNAPPY_DEPLOY_DIRECTORY
	"${COFFEE_DEPLOY_DIRECTORY}/${SNAPPY_PACKAGING_CAT}"
	CACHE PATH "" )
    set ( SNAPPY_OUTPUT_DIRECTORY
	"${COFFEE_PACKAGE_DIRECTORY}/${SNAPPY_PACKAGING_CAT}"
	CACHE PATH "" )
endif()

macro ( SNAPPY_TRANSLATE_ARCH_NAME OUTPUT_VAR )
    if("${CMAKE_LIBRARY_ARCHITECTURE}" STREQUAL "x86_64-linux-gnu")
        set(OUTPUT_VAR "amd64")
    elseif("${CMAKE_LIBRARY_ARCHITECTURE}" STREQUAL "i386-linux-gnu" OR
            "${CMAKE_LIBRARY_ARCHITECTURE}" STREQUAL "i686-linux-gnu")
        set(OUTPUT_VAR "i386")
    endif()
endmacro()

macro ( SNAPPY_TRANSLATE_PERMISSIONS PERMISSIONS_LIST PERMISSION_OUTPUT )
    set ( ${PERMISSION_OUTPUT}
#        "system-observe"
#        "hardware-observe"

#        "network"
#        "network-observe"
#        "network-bind"

#        "opengl"
#        "x11"

#        "pulseaudio"
        )
    foreach( PARAM ${PERMISSIONS_LIST} )
        get_permission_flag( ${PARAM} PARAM_ENABLED )
        if( "${PARAM_ENABLED}" STREQUAL "1" )
            set ( PARAM_TRANS )
            # Snapcraft does not like it if we have non-unique elements in the list, we therefore verify this
            if("${PARAM}" MATCHES "SIMPLE_GRAPHICS" OR "${PARAM}" MATCHES "OPENGL")
                # SDL2 needs to create a Unix socket, therefore we need this
                set ( PARAM_TRANS "x11;opengl;network-bind;network" )
            elseif("${PARAM}" MATCHES "AUDIO")
                set ( PARAM_TRANS "pulseaudio" )
            elseif("${PARAM}" MATCHES "JOYSTICK")
                set ( PARAM_TRANS "system-observe;hardware-observe" )
            elseif("${PARAM}" MATCHES "BROWSER")
                set ( PARAM_TRANS "browser-support" )
            elseif("${PARAM}" MATCHES "NETWORK_ACCESS"
                    AND NOT "${PERMISSIONS_LIST}" MATCHES ".*GRAPHICS.*"
                    AND NOT "${PERMISSIONS_LIST}" MATCHES ".*OPENGL.*"
                    AND NOT "${PERMISSIONS_LIST}" MATCHES ".*NETWORK_CONNECT.*"
                    AND NOT "${PERMISSIONS_LIST}" MATCHES ".*NETWORK_SERVE.*")
                set ( PARAM_TRANS "network" )
            elseif("${PARAM}" MATCHES "NETWORK_CONNECT"
                    OR "${PARAM}" MATCHES "NETWORK_SERVE")
                set ( PARAM_TRANS "network-observe" )
                if(NOT "${PERMISSIONS_LIST}" MATCHES ".*GRAPHICS.*"
                        AND NOT "${PERMISSIONS_LIST}" MATCHES ".*OPENGL.*")
                    set ( PARAM_TRANS "${PARAM_TRANS};network;network-bind" )
                endif()
            endif()
            list ( APPEND ${PERMISSION_OUTPUT} "${PARAM_TRANS}" )
        endif()
    endforeach()
endmacro()

macro ( SNAPPY_PACKAGE
        TARGET
        TITLE SUMMARY
        VERSION COPYRIGHT COMPANY
        DATA
        LIBRARIES LIBRARY_FILES
        EXEC_FILES
        ICON_ASSET
        PERMISSIONS
        )

    set ( SNAPPY_PACKAGE_NAME "${TARGET}" )

    string ( TOLOWER "${SNAPPY_PACKAGE_NAME}" SNAPPY_PACKAGE_NAME )
    string ( REPLACE "_" "-" SNAPPY_PACKAGE_NAME "${SNAPPY_PACKAGE_NAME}" )

    set ( SNAPPY_PKG_DIR "${SNAPPY_DEPLOY_DIRECTORY}/${TARGET}" )
    set ( SNAPPY_FINAL_SNAP "${SNAPPY_OUTPUT_DIRECTORY}/${SNAPPY_PACKAGE_NAME}_${VERSION}_all.snap" )
    set ( SNAPPY_ARCH_DATA
        "amd64) ARCH_STRING=\"x86_64-linux-gnu\" ;;\ni386) ARCH_STRING=\"i386-linux-gnu\" ;;\narmhf) ARCH_STRING=\"arm-linux-gnueabihf\" ;;"
    )

    set ( SNAPCRAFT_FILE "${SNAPPY_PKG_DIR}/snapcraft.yaml" )
    set ( ICON_TARGET "${SNAPPY_PKG_DIR}/meta/gui/icon.svg" )

    set ( SNAPPY_TITLE "${TITLE}" )
    set ( SNAPPY_VERSION "${VERSION}" )
    set ( SNAPPY_ARCHITECTURES "all" )
    set ( SNAPPY_SUMMARY "${SUMMARY}" )

    set ( SNAPPY_GRADE "devel" )
    set ( SNAPPY_CONFINEMENT "devmode" )

    # Retrieve Snappy permissions list
    snappy_translate_permissions( "${PERMISSIONS}" SNAPPY_PERMISSIONS )

    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SNAPPY_PKG_DIR}/snap/gui"
        )
    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SNAPPY_PKG_DIR}/meta/gui"
        )
    execute_process (
        COMMAND ${CMAKE_COMMAND} -E copy "${ICON_ASSET}" "${ICON_TARGET}"
        )
    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SNAPPY_OUTPUT_DIRECTORY}"
        )

    configure_file (
        "${SNAPPY_CONFIG_DIR}/snap.yaml.in"
        "${SNAPCRAFT_FILE}"
        @ONLY
        )

    configure_file (
        "${SNAPPY_CONFIG_DIR}/snap-select.sh"
        "${SNAPPY_PKG_DIR}/snap-select.sh"
        @ONLY
        )

    file ( APPEND "${SNAPCRAFT_FILE}"
        "apps:\n"
        "  game:\n"
        "    command: bin/${TARGET}\n"
        "    plugs:\n"
        )
    foreach( PERM ${SNAPPY_PERMISSIONS})
        file ( APPEND "${SNAPCRAFT_FILE}"
            "      - ${PERM}\n"
            )
    endforeach()
    file ( APPEND "${SNAPCRAFT_FILE}"
        "parts:\n"
        "  binary-import:\n"
        "    plugin: copy\n"
        "    files:\n"
	"      \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}\": \"bin/${CMAKE_LIBRARY_ARCHITECTURE}/${TARGET}\"\n"
        "      \"${SNAPPY_PKG_DIR}/snap-select.sh\": \"bin/${TARGET}\"\n"
        )

    foreach(LIB ${LIBRARY_FILES})
	get_filename_component ( SH_LIB "${LIB}" NAME )

        file ( APPEND "${SNAPCRAFT_FILE}"
            "      \"${LIB}\": \"lib/${CMAKE_LIBRARY_ARCHITECTURE}/${SH_LIB}\"\n")
    endforeach()

    foreach(BIN ${EXEC_FILES})
        file ( APPEND "${SNAPCRAFT_FILE}"
            "      \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BIN}\": \"bin/${CMAKE_LIBRARY_ARCHITECTURE}/${BIN}\"\n")
    endforeach()

    set ( NUM_IMPORTS "0" )
    foreach ( SRC ${DATA} )
        file ( APPEND "${SNAPCRAFT_FILE}"
            "  resc-import-${NUM_IMPORTS}:\n"
            "    plugin: dump\n"
            "    source: \"${SRC}\"\n"
            "    organize:\n"
            "      \'*\': assets/.\n"
            )
        math ( EXPR NUM_IMPORTS "${NUM_IMPORTS}+1" )
    endforeach()

    target_sources ( ${TARGET} PUBLIC
        "${SNAPCRAFT_FILE}"
        "${ICON_TARGET}"
        "${SNAPPY_PKG_DIR}/snap-select.sh"
        )

    set ( PYTHON_ENCODING_STUFF LC_ALL=C.UTF-8 LANG=C.UTF-8 )

    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${PYTHON_ENCODING_STUFF} ${SNAPPY_PROGRAM} clean
        WORKING_DIRECTORY ${SNAPPY_PKG_DIR}
        USES_TERMINAL
	)
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${PYTHON_ENCODING_STUFF} ${SNAPPY_PROGRAM} build
        WORKING_DIRECTORY ${SNAPPY_PKG_DIR}
        USES_TERMINAL
        )
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${PYTHON_ENCODING_STUFF} ${SNAPPY_PROGRAM} snap -o "${SNAPPY_FINAL_SNAP}"
        WORKING_DIRECTORY ${SNAPPY_PKG_DIR}
        USES_TERMINAL
        )

    install (
	FILES
	"${SNAPPY_FINAL_SNAP}"

	DESTINATION
	"${CMAKE_PACKAGED_OUTPUT_PREFIX}/${SNAPPY_PACKAGING_CAT}"
	)

endmacro()
