if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
    set ( RWIMAGE_DEPLOY "${COFFEE_DEPLOY_DIRECTORY}/linux-rwimage" )
    set ( RWIMAGE_PACKAGED "${COFFEE_PACKAGE_DIRECTORY}/linux-rwimage" )

    set ( RWIMAGE_DESKTOP "${COFFEE_DESKTOP_DIRECTORY}/linux/rwimage" )
endif()

function ( RWIMAGE_PACKAGE )

    set ( ONE_OPTS
        TARGET

        DOM_NAME
        TITLE

        VERSION_CODE
        COPYRIGHT
        COMPANY

        ICON
        )

    set ( MULTI_OPTS
        RESOURCES
        BUNDLE_LIBRARIES
        )

    cmake_parse_arguments ( RWI "" "${ONE_OPTS}" "${MULTI_OPTS}" ${ARGN} )

    set ( FINAL_FILE "${RWIMAGE_PACKAGED}/${RWI_TARGET}.rwi" )
    set ( BASE_DIR "${RWIMAGE_DEPLOY}/${RWI_TARGET}" )
    set ( MOUNT_DIR "${RWIMAGE_DEPLOY}/${RWI_TARGET}.mount" )

    add_custom_target ( ${RWI_TARGET}.rwi DEPENDS ${RWI_TARGET} )

    add_custom_command ( TARGET ${RWI_TARGET}.rwi
        PRE_BUILD

        COMMAND ${CMAKE_COMMAND} -E remove "${FINAL_FILE}"

        COMMAND ${CMAKE_COMMAND} -E make_directory "${BASE_DIR}/bin"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BASE_DIR}/lib"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BASE_DIR}/assets"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BASE_DIR}/store"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RWIMAGE_PACKAGED}"
        )

    set ( BIN_DIR "${BASE_DIR}/bin" )
    set ( LIB_DIR "${BASE_DIR}/lib" )
    set ( ASS_DIR "${BASE_DIR}/assets" )
    set ( SAVE_DIR "${BASE_DIR}/store" )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BASE_DIR}"
        )

    set ( SPECFILE "${RWIMAGE_DEPLOY}/${RWI_TARGET}.spec" )

    file ( WRITE "${SPECFILE}"
        "/set    type=dir mode=0555 uid=1 gid=1\n"
        ".       \n"
        "bin     \n"
        "lib     \n"
        "assets  mode=0444\n"
        "store   type=dir mode=0777 uid=999 gid=999\n"
        )

    configure_file (
        "${RWIMAGE_DESKTOP}/launch.sh.in"
        "${BASE_DIR}/launch.sh"
        @ONLY
        )

    configure_file (
        "${RWI_ICON}"
        "${BASE_DIR}/icon.svg"
        COPYONLY
        )

    configure_file (
        "${RWIMAGE_DESKTOP}/../appimage/set_icon.py"
        "${BASE_DIR}/set_icon.py"
        COPYONLY
        )

    foreach ( RESC ${RWI_RESOURCES} )
        add_custom_command ( TARGET ${RWI_TARGET}.rwi
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESC}" "${ASS_DIR}"
            )
    endforeach()
    foreach ( LIB ${RWI_BUNDLE_LIBRARIES} )
        add_custom_command ( TARGET ${RWI_TARGET}.rwi
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${LIB}" "${LIB_DIR}"
            )
    endforeach()

    if(TARGET CrashRecovery)
        add_custom_command ( TARGET ${RWI_TARGET}.rwi
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_FILE:CrashRecovery>"
                "${BIN_DIR}/CrashRecovery"
            )
    endif()

    add_custom_command ( TARGET ${RWI_TARGET}.rwi
        POST_BUILD
        # Copy in the executable
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:${RWI_TARGET}>"
            "${BIN_DIR}/${RWI_TARGET}"

        # Make space for the preloaded data
        COMMAND sh -c
            "truncate -s 128M ${FINAL_FILE}"
        # Extend it with 32M for writable storage + reserved for FS
        COMMAND sh -c
            "truncate -s +32M ${FINAL_FILE}"
        # Create the EXT2 filesystem in the file
        COMMAND mkfs.ext2 ${FINAL_FILE}
        # Write the files to the image in userspace
        COMMAND ${CMAKE_COMMAND} -E make_directory ${MOUNT_DIR}

        COMMAND fuseext2 ${FINAL_FILE} ${MOUNT_DIR} -o rw+

        # Populate the image
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${BASE_DIR}/ ${MOUNT_DIR}/
        COMMAND ${CMAKE_COMMAND} -E make_directory ${MOUNT_DIR}/store
        # Set permissions
        COMMAND chmod -R 0555
            ${MOUNT_DIR}/bin ${MOUNT_DIR}/lib ${MOUNT_DIR}/assets
        COMMAND chmod -R 0777 ${MOUNT_DIR}/store

        COMMAND fusermount -u ${MOUNT_DIR}
        )
endfunction()
