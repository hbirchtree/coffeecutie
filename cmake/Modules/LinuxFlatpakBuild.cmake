if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set ( FLATPAK_CONFIG_DIR "${CMAKE_SOURCE_DIR}/desktop/linux/flatpak" )

    set ( FLATPAK_REPOSITORY_DIR CACHE PATH "Target repository to submit flatpaks to" )

    set ( FLATPAK_PROGRAM "/usr/bin/flatpak" CACHE FILEPATH "Path to flatpak executable" )
    set ( FLATPAK_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/package/linux-flatpak"
        CACHE PATH "Where to put flatpak directory structures" )

    set ( FLATPAK_DEFAULT_ICON_FILE "${CMAKE_SOURCE_DIR}/desktop/icon.svg"
        CACHE FILEPATH "Default icon for AppImages" )
endif()

macro( FLATPAK_PACKAGE TARGET DOM_NAME TITLE VERSION_CODE COPYRIGHT COMPANY DATA LIBRARIES BUNDLE_LIBRARIES )

    set ( FLATPAK_TARGET_BRANCH "master" )

    set ( FLATPAK_PKG_NAME "${DOM_NAME}.${TARGET}" )

    set ( FLATPAK_TITLE "${TITLE}" )
    set ( FLATPAK_EXEC "${TARGET}" )
    set ( FLATPAK_EXEC_WM "${TARGET}" )

    set ( FLATPAK_BASE_DIR "${FLATPAK_WORKING_DIRECTORY}/${FLATPAK_PKG_NAME}" )

    set ( FLATPAK_ASSET_DIR "${FLATPAK_BASE_DIR}/files/assets" )
    set ( FLATPAK_LIBRARY_DIR "${FLATPAK_BASE_DIR}/files/lib" )
    set ( FLATPAK_BINARY_DIR "${FLATPAK_BASE_DIR}/files/bin" )
    set ( FLATPAK_EXPORT_DIR "${FLATPAK_BASE_DIR}/export" )

    set ( FLATPAK_ICON_REF "${FLATPAK_PKG_NAME}.svg" )

    # Create directory structures
    add_custom_command ( TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_ASSET_DIR}"
        )

    add_custom_command ( TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_BINARY_DIR}"
        )

    add_custom_command ( TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_LIBRARY_DIR}"
        )

    add_custom_command ( TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_EXPORT_DIR}"
        )

    # Configure metadata file
    configure_file (
        "${FLATPAK_CONFIG_DIR}/metadata.in"
        "${FLATPAK_BASE_DIR}/metadata"
        @ONLY
        )
    configure_file (
        "${FLATPAK_CONFIG_DIR}/application.desktop.in"
        "${FLATPAK_EXPORT_DIR}/${FLATPAK_PKG_NAME}.desktop"
        @ONLY
        )
    configure_file (
        "${FLATPAK_DEFAULT_ICON_FILE}"
        "${FLATPAK_BASE_DIR}/export/${FLATPAK_ICON_REF}"
        COPYONLY
        )

    # Copy resources into flatpak
    foreach ( RESC ${DATA} )
        add_custom_command ( TARGET ${TARGET}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESC}" "${FLATPAK_ASSET_DIR}"
            )
    endforeach()

    # Copy bundled libraries into flatpak
    foreach ( LIB ${BUNDLE_LIBRARIES} )
        add_custom_command ( TARGET ${TARGET}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${LIB}" "${FLATPAK_LIBRARY_DIR}"
            )
    endforeach()

    foreach ( LIB ${LIBRARIES} )
        add_custom_command ( TARGET ${TARGET}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${LIB}>" "${FLATPAK_LIBRARY_DIR}"
            )
    endforeach()

    # Finally, copy binary into flatpak
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET}>" "${FLATPAK_BINARY_DIR}"
        )

    # Add arrangement to install flatpak structure somewhere else
    install (
        DIRECTORY
        "${FLATPAK_BASE_DIR}"

        DESTINATION
        "${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-flatpak"
        )

endmacro(FLATPAK_PACKAGE)
