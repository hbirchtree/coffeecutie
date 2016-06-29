macro(WINPE_PACKAGE TARGET SOURCES RESOURCES)
    # TODO: Generate Win32 resources here for extraction in-memory

	set ( RESOURCE_DESCRIPTOR ${CMAKE_CURRENT_BINARY_DIR}/custom_data.rc )

	file ( WRITE "${RESOURCE_DESCRIPTOR}" "// Automatically generated resource file by Coffee \r\n" )

	foreach(durr ${RESOURCES})
        file(GLOB_RECURSE TMP ${durr}/* )
        foreach(file_full ${TMP})
            file ( RELATIVE_PATH file_dir ${durr} ${file_full} )
            get_filename_component ( file_dir "${file_dir}" DIRECTORY )
            get_filename_component ( file_name "${file_full}" NAME )
			string(TOLOWER "${file_name}" file_name_lower)
            if(NOT ("${file_name_lower}" STREQUAL "thumbs.db"))
                file (
					APPEND "${RESOURCE_DESCRIPTOR}"
					"\"${file_dir}${file_name}\" CF_RES \"${file_full}\" \r\n"
					)
            endif()
        endforeach()
    endforeach()

    add_executable(${TARGET} ${SOURCES} ${CMAKE_SOURCE_DIR}/desktop/windows/winresources.rc ${RESOURCE_DESCRIPTOR} )

    set_target_properties ( ${TARGET}
        PROPERTIES
        VERSION ${COFFEE_BUILD_STRING}
        SOVERSION 1
        )
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