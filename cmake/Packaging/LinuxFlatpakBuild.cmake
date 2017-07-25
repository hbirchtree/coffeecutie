if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set ( FLATPAK_CONFIG_DIR "${COFFEE_DESKTOP_DIRECTORY}/linux/flatpak" )

    set ( FLATPAK_REPOSITORY_DIR CACHE PATH "Target repository to submit flatpaks to" )

    set ( FLATPAK_PROGRAM "/usr/bin/flatpak" CACHE FILEPATH "Path to flatpak executable" )
    set ( FLATPAK_WORKING_DIRECTORY "${COFFEE_DEPLOY_DIRECTORY}/linux-flatpak"
        CACHE PATH "Where to put flatpak directory structures" )
    set ( FLATPAK_DEPLOY_DIRECTORY "${COFFEE_DEPLOY_DIRECTORY}/linux-flatpak"
        CACHE PATH "Where to put flatpak directory structures" )

    set ( FLATPAK_DEFAULT_ICON_FILE "${COFFEE_DESKTOP_DIRECTORY}/icon.svg"
        CACHE FILEPATH "Default icon for AppImages" )

    set ( FLATPAK_DEPLOY_LOCALLY FALSE CACHE BOOL "Deploy with local, user-only repository" )
endif()

macro( FLATPAK_PACKAGE
        TARGET
        DOM_NAME TITLE VERSION_CODE COPYRIGHT COMPANY
        DATA
        LIBRARIES BUNDLE_LIBRARIES
        ICON_ASSET )

    set ( FLATPAK_TARGET_BRANCH "master" )

    set ( FLATPAK_PKG_NAME "${DOM_NAME}.${TARGET}" )

    set ( FLATPAK_TITLE "${TITLE}" )
    set ( FLATPAK_EXEC "${TARGET}" )
    set ( FLATPAK_EXEC_WM "${TARGET}" )
    set ( FLATPAK_ARCH "x86_64" )

    if("${CMAKE_LIBRARY_ARCHITECTURE}" STREQUAL "i386-linux-gnu" OR
            "${CMAKE_LIBRARY_ARCHITECTURE}" STREQUAL "i686-linux-gnu")
        set ( FLATPAK_ARCH "i386" )
    elseif("${CMAKE_LIBRARY_ARCHITECTURE}" MATCHES "aarch64")
        set ( FLATPAK_ARCH "aarch64" )
    elseif("${CMAKE_LIBRARY_ARCHITECTURE}" MATCHES "arm")
        set ( FLATPAK_ARCH "arm" )
    endif()

    set ( FLATPAK_BASE_DIR "${FLATPAK_DEPLOY_DIRECTORY}/${FLATPAK_PKG_NAME}" )

    set ( FLATPAK_ASSET_DIR "${FLATPAK_BASE_DIR}/files/assets" )
    set ( FLATPAK_LIBRARY_DIR "${FLATPAK_BASE_DIR}/files/lib" )
    set ( FLATPAK_BINARY_DIR "${FLATPAK_BASE_DIR}/files/bin" )
    set ( FLATPAK_EXPORT_DIR "${FLATPAK_BASE_DIR}/export" )

    set ( FLATPAK_ICON_REF "${FLATPAK_PKG_NAME}.svg" )

    set ( FLATPAK_BUNDLE_REPO "${FLATPAK_DEPLOY_DIRECTORY}/${TARGET}" )
    set ( FLATPAK_BUNDLE_DIR "${FLATPAK_WORKING_DIRECTORY}/${TARGET}.flatpak" )

    # TODO: Unify this with the in-app information somehow
    set ( FLATPAK_CONFIG "${TARGET}" )

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

    add_custom_command ( TARGET ${TARGET}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_DEPLOY_DIRECTORY}"
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
        "${ICON_ASSET}"
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

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        add_custom_command ( TARGET ${TARGET}
            POST_BUILD
            COMMAND strip "${FLATPAK_BINARY_DIR}/${TARGET}"
            )
    endif()

    add_custom_command ( TARGET ${TARGET}
	POST_BUILD
	COMMAND ${FLATPAK_PROGRAM} build-export "${FLATPAK_BUNDLE_REPO}" "${FLATPAK_BASE_DIR}"
	)

    add_custom_command ( TARGET ${TARGET}
	POST_BUILD
        COMMAND ${FLATPAK_PROGRAM} build-bundle "${FLATPAK_BUNDLE_REPO}" "${FLATPAK_BUNDLE_DIR}" "${FLATPAK_PKG_NAME}" )

    if(FLATPAK_DEPLOY_LOCALLY)
        add_custom_command( TARGET ${TARGET}
            POST_BUILD
            COMMAND ${FLATPAK_PROGRAM} remote-add --user --no-gpg-verify deployed_${FLATPAK_PKG_NAME} ${FLATPAK_BUNDLE_REPO} || true
            )
        add_custom_command( TARGET ${TARGET}
            POST_BUILD
            COMMAND ${FLATPAK_PROGRAM} uninstall --user ${FLATPAK_PKG_NAME} || true
            )
        add_custom_command( TARGET ${TARGET}
            POST_BUILD
            COMMAND ${FLATPAK_PROGRAM} install --user deployed_${FLATPAK_PKG_NAME} ${FLATPAK_PKG_NAME}
            )
    endif()

    # Add arrangement to install flatpak structure somewhere else
#    install (
#        DIRECTORY
#        "${FLATPAK_BASE_DIR}"

#        DESTINATION
#	"${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-flatpak"
#        )
    install (
        FILES
        "${FLATPAK_BUNDLE_DIR}"

	DESTINATION
	"${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-flatpak"
	)

endmacro(FLATPAK_PACKAGE)
