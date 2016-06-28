macro(WINPE_PACKAGE TARGET SOURCES RESOURCES)
    # TODO: Generate Win32 resources here for extraction in-memory

	message( "Resources to be packed: ${RESOURCES}" )

    add_executable(${TARGET} ${SOURCES} ${CMAKE_SOURCE_DIR}/desktop/windows/winresources.rc )

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