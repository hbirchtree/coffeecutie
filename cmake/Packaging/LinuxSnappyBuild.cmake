if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
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

macro ( SNAPPY_PACKAGE
        TARGET
        TITLE SUMMARY
        VERSION COPYRIGHT COMPANY
        DATA
        LIBRARIES LIBRARY_FILES
        ICON_ASSET)

    set ( SNAPPY_PACKAGE_NAME "${TARGET}" )
    string ( TOLOWER "${SNAPPY_PACKAGE_NAME}" SNAPPY_PACKAGE_NAME )
    string ( REPLACE "_" "-" SNAPPY_PACKAGE_NAME "${SNAPPY_PACKAGE_NAME}" )

    set ( SNAPPY_PKG_DIR "${SNAPPY_DEPLOY_DIRECTORY}/${TARGET}" )
    set ( SNAPPY_FINAL_SNAP "${SNAPPY_OUTPUT_DIRECTORY}/${SNAPPY_PACKAGE_NAME}_${VERSION}_all.snap" )

    set ( ICON_TARGET "${SNAPPY_PKG_DIR}/setup/gui/icon.svg" )

    set ( SNAPPY_TITLE "${TITLE}" )
    set ( SNAPPY_VERSION "${VERSION}" )
    set ( SNAPPY_ARCHITECTURES "all" )
    set ( SNAPPY_SUMMARY "${SUMMARY}" )

    set ( SNAPPY_CONFINEMENT "strict" )

    if( "${CMAKE_BUILD_TYPE}" MATCHES "Debug" )
        set ( SNAPPY_CONFINEMENT "devmode" )
    endif()

    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SNAPPY_PKG_DIR}/setup/gui"
        )
    execute_process (
        COMMAND ${CMAKE_COMMAND} -E copy "${ICON_ASSET}" "${ICON_TARGET}"
        )
    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SNAPPY_OUTPUT_DIRECTORY}"
        )

    set ( SNAPCRAFT_FILE "${SNAPPY_PKG_DIR}/snapcraft.yaml" )

    set ( SNAPPY_ARCH_DATA "amd64) ARCH_STRING=\"${CMAKE_LIBRARY_ARCHITECTURE}\" ;;" )

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
        "      - network\n"
        "      - opengl\n"
        "      - pulseaudio\n"
        "      - x11\n"
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

    set ( NUM_IMPORTS "0" )
    foreach ( SRC ${DATA} )
        file ( APPEND "${SNAPCRAFT_FILE}"
            "  resc-import-${NUM_IMPORTS}:\n"
            "    plugin: copy\n"
            "    source: \"${SRC}\"\n"
            "    files:\n"
            "      \"*\": assets/.\n"
            )
        math ( EXPR NUM_IMPORTS "${NUM_IMPORTS}+1" )
    endforeach()

    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${SNAPPY_PROGRAM} clean
        WORKING_DIRECTORY ${SNAPPY_PKG_DIR}
	)
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${SNAPPY_PROGRAM} build
        WORKING_DIRECTORY ${SNAPPY_PKG_DIR}
        )
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${SNAPPY_PROGRAM} snap -o "${SNAPPY_FINAL_SNAP}"
        WORKING_DIRECTORY ${SNAPPY_PKG_DIR}
        )

    install (
	FILES
	"${SNAPPY_FINAL_SNAP}"

	DESTINATION
	"${CMAKE_PACKAGED_OUTPUT_PREFIX}/${SNAPPY_PACKAGING_CAT}"
	)

endmacro()
