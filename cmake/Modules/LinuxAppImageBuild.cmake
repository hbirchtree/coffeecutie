set ( APPIMAGE_CONFIG_DIR "${CMAKE_SOURCE_DIR}/desktop/linux/appimage" )
set ( APPIMAGE_ASSISTANT_PROGRAM CACHE FILEPATH "AppImageAssistant executable" )

macro( APPIMAGE_PACKAGE TARGET APPIMAGE_TITLE DATA LIBRARIES LIBRARY_FILES )
    set ( APPIMAGE_TITLE "${APPIMAGE_TITLE}" )
    string ( TOLOWER "${APPIMAGE_TITLE}" APPIMAGE_INTERNALNAME )
    set ( APPIMAGE_ICON "${CMAKE_SOURCE_DIR}/desktop/icon.svg" )
    set ( APPIMAGE_LIBRARIES )
    set ( APPIMAGE_DATA )

    set ( APPIMAGE_EXEC ${TARGET} )

    message ( "AppImage executable: ${APPIMAGE_EXEC}" )

    set ( APPIMAGE_CACHE_DIR "${CMAKE_BINARY_DIR}/deploy/linux-appimage/${APPIMAGE_INTERNALNAME}_cache" )
    set ( APPIMAGE_INTERMEDIATE_DIR "${CMAKE_BINARY_DIR}/deploy/linux-appimage/${APPIMAGE_INTERNALNAME}" )
    set ( APPIMAGE_ASSET_DIR "${APPIMAGE_INTERMEDIATE_DIR}/assets" )
    set ( APPIMAGE_FINAL_NAME "${CMAKE_BINARY_DIR}/package/linux-appimage/${APPIMAGE_TITLE}.AppImage" )

    list ( APPEND APPIMAGE_LIBRARIES
        ${LIBRARIES} )
    list ( APPEND APPIMAGE_DATA
        ${DATA} )

    add_custom_command (
        TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove "${APPIMAGE_FINAL_NAME}"
        )

    # Create some necessary directory structure in AppDir
    add_custom_command (
        TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_INTERMEDIATE_DIR}/lib"
        )
    add_custom_command (
        TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_INTERMEDIATE_DIR}/assets"
        )

    add_custom_command (
        TARGET ${TARGET}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/package/linux-appimage/"
        )

    # Copy and configure some data for the AppDir
    configure_file (
        "${APPIMAGE_ICON}"
        "${APPIMAGE_INTERMEDIATE_DIR}/.DirIcon"
        COPYONLY
        )
    configure_file (
        "${APPIMAGE_ICON}"
        "${APPIMAGE_INTERMEDIATE_DIR}"
        COPYONLY
        )
    file (
        COPY "${APPIMAGE_CONFIG_DIR}/startup.sh.in"
        DESTINATION "${APPIMAGE_CACHE_DIR}"
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        )
    configure_file (
        "${APPIMAGE_CACHE_DIR}/startup.sh.in"
        "${APPIMAGE_INTERMEDIATE_DIR}/AppRun"
        @ONLY
        )
    configure_file (
        "${APPIMAGE_CONFIG_DIR}/application.desktop.in"
        "${APPIMAGE_INTERMEDIATE_DIR}/appname.desktop"
        @ONLY
        )

    # Copy resources into AppDir
    foreach ( RESC ${DATA} )
        add_custom_command ( TARGET ${TARGET}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESC} ${APPIMAGE_ASSET_DIR}
            )
    endforeach()

    # Copy the binary to AppDir
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET}>" "${APPIMAGE_INTERMEDIATE_DIR}"
        )

    # Do the actual packaging step with AppImageKit
    add_custom_command (
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${APPIMAGE_ASSISTANT_PROGRAM} ${APPIMAGE_INTERMEDIATE_DIR} ${APPIMAGE_FINAL_NAME}
        )

    install (
        FILES
        ${APPIMAGE_FINAL_NAME}

        DESTINATION
        ${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-appimage
        )
endmacro()
