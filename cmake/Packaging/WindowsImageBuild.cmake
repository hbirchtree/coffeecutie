macro(WINPE_PACKAGE
        TARGET
        DOM_NAME TITLE VERSION_CODE COPYRIGHT COMPANY
        INFO_STRING
        SOURCES RESOURCES
        ICON_ASSET )

	set ( INCLUDED_LIBS "" )
	# Locate necessary binary files
	foreach(lib ${ANGLE_LIBRARIES};${SDL2_LIBRARY})
		get_filename_component ( LIB_BASE "${lib}" NAME_WE )
		get_filename_component ( LIB_DIR "${lib}" DIRECTORY )

		set ( INCLUDED_LIBS "${INCLUDED_LIBS};${LIB_DIR}/${LIB_BASE}.dll" )
	endforeach()

    set ( WINDOWS_DIST_COMPANY "${COMPANY}" )

    set ( WINDOWS_PACKAGE_NAME "${DOM_NAME}.${TARGET}" )
    set ( WINDOWS_PACKAGE_VERSION "${VERSION_CODE}.0.0.0" )

    set ( WINDOWS_APP_TITLE "${TITLE}" )
    set ( WINDOWS_APP_DESCRIPTION "${INFO_STRING}" )
    set ( WINDOWS_APP_INTERNALNAME "${TARGET}" )
    set ( WINDOWS_APP_EXECNAME "${TARGET}.exe" )

    set ( WINDOWS_APP_FILEVER "${VERSION_CODE}, 0, 0, 0" )
    set ( WINDOWS_APP_VERSION "${VERSION_CODE}, 0, 0, 0" )

    set ( WINDOWS_APP_COMPANY "${WINDOWS_DIST_COMPANY}" )
    set ( WINDOWS_APP_COPYRIGHT "${COPYRIGHT}")
    
    # We describe resources in a .rc file for in-memory access
    set ( RESOURCE_DESCRIPTOR ${CMAKE_CURRENT_BINARY_DIR}/custom_data.rc )
    set ( RESOURCE_HEADER ${CMAKE_CURRENT_BINARY_DIR}/custom_data.c )

    file ( WRITE "${RESOURCE_DESCRIPTOR}" "// Automatically generated resource file by Coffee \r\n" )

    set ( RESC_NUM "0" )

    # Clear resource descriptor to avoid dupes
    file ( WRITE "${RESOURCE_DESCRIPTOR}"   "" )
    # Clear resource header
    file ( WRITE "${RESOURCE_HEADER}"       "" )

    if(NOT WIN_UWP)
        foreach(durr ${RESOURCES})
            file(GLOB_RECURSE TMP ${durr}/* )
            foreach(file_full ${TMP})
                # First, get a relative filename
                # This is used to describe structure
                file ( RELATIVE_PATH file_dir ${durr} ${file_full} )
                get_filename_component ( file_dir "${file_dir}" DIRECTORY )
                get_filename_component ( file_name "${file_full}" NAME )
                # We get a lower-case version to compare with other filenames
                string(TOLOWER "${file_name}" file_name_lower)

                if(NOT ("${file_name_lower}" STREQUAL "thumbs.db"))
                    file (
                        APPEND "${RESOURCE_HEADER}"
                        "{${RESC_NUM},\"${file_dir}${file_name}\"},"
                        )
					# If there is a directory path, append a "_" for it to be correct
                    # This is disgusting.
					if(file_dir)
                        set( file_dir "${file_dir}/" )
                    endif()
					# Set virtual filename
					set ( virt_fname "${file_dir}${file_name}" )
                    string ( REPLACE "_" "___" virt_fname "${virt_fname}" )
                    string ( REPLACE "/" "_" virt_fname "${virt_fname}" )
                    string ( REPLACE "\\" "_" virt_fname "${virt_fname}" )
                    # Insert the file with directory path and filename into the .rc file
                    file (
                        APPEND "${RESOURCE_DESCRIPTOR}"
                        "\"${virt_fname}\" CF_RES \"${file_full}\" \r\n"
                        )
                endif()
                # Increment resource number, inserted into .rc file
                math ( EXPR RESC_NUM "${RESC_NUM} + 1" )
            endforeach()
        endforeach()
    endif()

    # We add an application manifest to get on the good side with Windows 8.1+
    set ( WINDOWS_DESKTOP_DIR "${COFFEE_DESKTOP_DIRECTORY}/windows" )

    set ( WINDOWS_BASE_SOURCE_RESOURCE "${WINDOWS_DESKTOP_DIR}/winresources.rc" )
    set ( WINDOWS_BASE_RESOURCE "winresource.rc" )
    set ( MANIFEST_SOURCE_FILE "${WINDOWS_DESKTOP_DIR}/template.manifest.in" )
    set ( MANIFEST_FILE "${TARGET}.exe.manifest" )

    configure_file (
        "${MANIFEST_SOURCE_FILE}"
        "${MANIFEST_FILE}"
        @ONLY
        )

    configure_file (
        "${WINDOWS_BASE_SOURCE_RESOURCE}"
        "${WINDOWS_BASE_RESOURCE}"
        @ONLY
        )

    configure_file (
        "${WINDOWS_DESKTOP_DIR}/icon.ico"
        "icon.ico"
        COPYONLY
        )

    # Finally we stir the smelly gak into PE

	if(WIN_UWP)
		set ( APPX_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/AppX" )
        include_directories( ${SDL2_INCLUDE_DIR} )
		include_directories( ${ANGLE_INCLUDE_DIR} )
	endif()

	
	if(MSVC AND WIN_UWP)
		set ( OPTIONS WIN32 )
	endif()

    add_executable(${TARGET}
		${OPTIONS}
		${SDL2_MAIN_C_FILE}
		${SOURCES}
        ${WINDOWS_BASE_RESOURCE}
        ${RESOURCE_DESCRIPTOR}
        ${MANIFEST_FILE}
		${INCLUDED_LIBS}
        )

	if(WIN_UWP)
		# I need these seeds for mye research, Morty, gotta stuff it waaay up there, Morty.
		target_link_libraries ( ${TARGET} ${SDL2_LIBRARY} )
		set_source_files_properties ( ${SDL2_MAIN_C_FILE}
			PROPERTIES COMPILE_FLAGS /ZW
			)
		#set_target_properties ( ${TARGET}
		#	PROPERTIES
		#	RESOURCE "${SDL2_LIBRARY_BIN};${ANGLE_LIBRARIES_BIN}"
		#	)
		execute_process ( COMMAND cmake -E make_directory ${APPX_DIR} )
		foreach(var ${INCLUDED_LIBS})
			set ( APPX_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/AppX" )
			configure_file(
				"${var}"
				${APPX_DIR}/
				COPYONLY
				)
			set ( APPX_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/AppX" )
			configure_file(
				"${var}"
				${APPX_DIR}/
				COPYONLY
				)
		endforeach()
	endif()

    set_target_properties ( ${TARGET}
        PROPERTIES
        VERSION ${COFFEE_BUILD_STRING}
        SOVERSION 1
        )
	if(NOT WIN_UWP)
		target_link_libraries ( ${TARGET}
			user32
			gdi32
			winmm
			imm32
			ole32
			oleaut32
			version
			uuid
			dinput8
			)
	endif()
    install(
        TARGETS
        ${TARGET}

        DESTINATION
        bin
        )
    install(
        FILES ${BUNDLE_LIBS}
        DESTINATION bin
        )
endmacro()
