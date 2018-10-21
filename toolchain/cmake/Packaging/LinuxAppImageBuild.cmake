if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set ( APPIMAGE_CONFIG_DIR "${COFFEE_DESKTOP_DIRECTORY}/linux/appimage" )

    set ( MKSQUASH_PROGRAM CACHE FILEPATH "AppImageAssistant executable" )
    set ( APPIMAGE_APPRUN_PROGRAM CACHE FILEPATH "AppImage AppRun executable" )
    set ( APPIMAGE_RUNTIME_BINARY CACHE FILEPATH "AppImage runtime to be embedded into the AppImage" )

    set ( APPIMAGE_WORKING_DIRECTORY "${COFFEE_DEPLOY_DIRECTORY}/linux-appimage"
        CACHE PATH "Where to put the AppDir items" )
    set ( APPIMAGE_OUTPUT_DIRECTORY "${COFFEE_PACKAGE_DIRECTORY}/linux-appimage"
        CACHE PATH "AppImage output directory" )
    set ( APPIMAGE_FOLLOW_STANDARD OFF
        CACHE BOOL "Whether generator should follow the spec" )

    set ( APPIMAGE_DEFAULT_ICON_FILE
        "${COFFEE_DESKTOP_DIRECTORY}/icon.svg"
        CACHE FILEPATH "Default icon for AppImages" )
endif()

macro( APPIMAGE_PACKAGE
        TARGET
        APPIMAGE_TITLE
        DATA
        LIBRARIES LIBRARY_FILES
        ICON_ASSET )
    string ( TOLOWER "${APPIMAGE_TITLE}" APPIMAGE_INTERNALNAME )
    string ( MAKE_C_IDENTIFIER "${APPIMAGE_INTERNALNAME}" APPIMAGE_INTERNALNAME )

    # Some prerequisites
    # TITLE here is used as the name of the final AppImage as well as the desktop entry's name
    set ( APPIMAGE_TITLE "${APPIMAGE_TITLE}" )
    set ( APPIMAGE_INTERNALNAME "${APPIMAGE_INTERNALNAME}" )
    set ( APPIMAGE_LIBRARIES )
    set ( APPIMAGE_DATA )

    # Icon file to be used for the AppImage, only one in this case, preferrably SVG
    set ( APPIMAGE_ICON "${ICON_ASSET}" )
    # We define a way to reference this icon based on where it is located
    set ( APPIMAGE_ICON_REF "${APPIMAGE_INTERNALNAME}.svg" )

    # This helps the window manager to recognize the program even if it has no embedded or loaded icon
    set ( APPIMAGE_EXEC_WM ${TARGET} )

    # Sets the launch variable in .desktop entry
    set ( APPIMAGE_EXEC ${TARGET} )

    # This directory is used for temporary files, might get messy
    set ( APPIMAGE_CACHE_DIR "${APPIMAGE_WORKING_DIRECTORY}/${APPIMAGE_INTERNALNAME}_cache" )
    # Where the AppDir is generated
    set ( APPIMAGE_INTERMEDIATE_DIR
        "${APPIMAGE_WORKING_DIRECTORY}/${APPIMAGE_INTERNALNAME}.AppDir" )
    set ( APPIMAGE_INTERMEDIATE_SQUASH
        "${APPIMAGE_WORKING_DIRECTORY}/${APPIMAGE_INTERNALNAME}.squashfs" )

    set ( APPIMAGE_ICON_TARGET
        "${APPIMAGE_INTERMEDIATE_DIR}/${APPIMAGE_ICON_REF}" )

    if(NOT APPIMAGE_FOLLOW_STANDARD)
        set ( APPIMAGE_BINARY_DIR "${APPIMAGE_INTERMEDIATE_DIR}" )
        set ( APPIMAGE_ASSET_DIR "${APPIMAGE_INTERMEDIATE_DIR}/assets" )
        set ( APPIMAGE_LIBRARY_DIR "${APPIMAGE_INTERMEDIATE_DIR}/lib" )
    else()
        set ( APPIMAGE_BINARY_DIR "${APPIMAGE_INTERMEDIATE_DIR}/usr/bin" )
        set ( APPIMAGE_ASSET_DIR "${APPIMAGE_INTERMEDIATE_DIR}/usr/share" )
        set ( APPIMAGE_LIBRARY_DIR "${APPIMAGE_INTERMEDIATE_DIR}/usr/lib" )
    endif()
    set ( APPIMAGE_FINAL_NAME "${APPIMAGE_OUTPUT_DIRECTORY}/${APPIMAGE_TITLE}.AppImage" )

    list ( APPEND APPIMAGE_LIBRARIES
        ${LIBRARIES} )
    list ( APPEND APPIMAGE_DATA
        ${DATA} )

    add_custom_command ( TARGET ${TARGET}
        PRE_BUILD

        # Remove the previous AppImage file to avoid confusion when generating a new one
        COMMAND ${CMAKE_COMMAND} -E remove "${APPIMAGE_FINAL_NAME}"

        # Create some necessary directory structure in AppDir
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_OUTPUT_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_CACHE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_LIBRARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_ASSET_DIR}"
        )

    # Copy and configure some data for the AppDir
    configure_file (
        "${APPIMAGE_ICON}"
        "${APPIMAGE_ICON_TARGET}"
        COPYONLY
        )
    configure_file (
        "${APPIMAGE_ICON}"
        "${APPIMAGE_INTERMEDIATE_DIR}/.DirIcon"
        COPYONLY
        )
    configure_file (
        "${APPIMAGE_CONFIG_DIR}/set_icon.py"
        "${APPIMAGE_INTERMEDIATE_DIR}/set_icon.py"
        COPYONLY
        )
    configure_file (
        "${APPIMAGE_CONFIG_DIR}/application.desktop.in"
        "${APPIMAGE_INTERMEDIATE_DIR}/${APPIMAGE_INTERNALNAME}.desktop"
        @ONLY
        )
    if(NOT APPIMAGE_FOLLOW_STANDARD)
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
    else()
        configure_file (
            "${APPIMAGE_APPRUN_PROGRAM}"
            "${APPIMAGE_INTERMEDIATE_DIR}/AppRun"
            COPYONLY
            )
    endif()

    # Copy resources into AppDir
    foreach ( RESC ${DATA} )
        add_custom_command ( TARGET ${TARGET}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESC}" "${APPIMAGE_ASSET_DIR}"
            )
    endforeach()

    # Copy bundled libraries into AppDir
    foreach ( LIB ${LIBRARY_FILES} )
        add_custom_command ( TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${LIB}" "${APPIMAGE_LIBRARY_DIR}"
            )
    endforeach()

    foreach ( LIB ${LIBRARIES} )
        add_custom_command ( TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${LIB}>" "${APPIMAGE_LIBRARY_DIR}"
            )
    endforeach()

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND
            (NOT "${LIBRARIES}" STREQUAL "" OR NOT "${LIBRARY_FILES}" STREQUAL ""))
        add_custom_command ( TARGET ${TARGET}
            POST_BUILD
            COMMAND bash -c '${CMAKE_STRIP} `find ${APPIMAGE_LIBRARY_DIR} -type f`'
            )
    endif()

    # Copy the binary to AppDir
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET}>" "${APPIMAGE_BINARY_DIR}"
        )

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        add_custom_command ( TARGET ${TARGET}
            POST_BUILD
            COMMAND strip "${APPIMAGE_BINARY_DIR}/${TARGET}"
            )
    endif()

    # Do the actual packaging
    add_custom_command ( TARGET ${TARGET}
        POST_BUILD
        USES_TERMINAL

        # First create squashfs
        COMMAND "${MKSQUASH_PROGRAM}" "${APPIMAGE_INTERMEDIATE_DIR}"
                "${APPIMAGE_INTERMEDIATE_SQUASH}"
                -root-owned -noappend
        # Concatenate AppImage runtime with image
        COMMAND cat "${APPIMAGE_RUNTIME_BINARY}" >> "${APPIMAGE_FINAL_NAME}"
        COMMAND cat "${APPIMAGE_INTERMEDIATE_SQUASH}" >> "${APPIMAGE_FINAL_NAME}"

        # Make image executable
        COMMAND chmod +x "${APPIMAGE_FINAL_NAME}"
        )

    install (
        FILES
        "${APPIMAGE_FINAL_NAME}"

        DESTINATION
        "${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-appimage"

        PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        )
endmacro()
