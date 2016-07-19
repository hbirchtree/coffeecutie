if(APPLE)
    set ( OSX_DEFAULT_ICON "${COFFEE_DESKTOP_DIRECTORY}/osx/Coffee.icns" CACHE FILEPATH "" )
endif()

macro( MACFRAMEWORK_PACKAGE
	TARGET LINKOPT
	VERSION_CODE COPYRIGHT COMPANY
	SOURCES BUNDLE_RSRCS BUNDLE_HEADERS)

    add_definitions( -DCOFFEE_APPLICATION_LIBRARY )

    add_library(${TARGET} ${LINKOPT} "${SOURCES}")

    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

    set_target_properties( ${TARGET} PROPERTIES
	MACOSX_RPATH "."
	FRAMEWORK ON
	FRAMEWORK_VERSION ${VERSION_CODE}
	)

    install(
	TARGETS
	${TARGET}

	DESTINATION
	frameworks
	)
endmacro()

macro( MACAPP_PACKAGE
        TARGET
        TITLE VERSION_CODE COPYRIGHT COMPANY
        INFO_STRING
        SOURCES BUNDLE_RSRCS BUNDLE_LIBS
        OSX_ICON )

    if(IOS)
        set ( RESOURCE_DIR "" )
    else()
        set ( RESOURCE_DIR "Resources" )
    endif()

    set_source_files_properties ( ${OSX_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "${RESOURCE_DIR}" )

    set ( MACOSX_BUNDLE_LONG_VERSION_STRING ${COFFEE_BUILD_STRING} )
    set ( MACOSX_BUNDLE_BUNDLE_VERSION ${COFFEE_BUILD_STRING} )
    set ( MACOSX_BUNDLE_SHORT_VERSION_STRING ${COFFEE_VERSION_CODE} )
    get_filename_component ( OSX_ICON_NAME "${OSX_ICON}" NAME )
    set ( MACOSX_BUNDLE_ICON_FILE "${OSX_ICON_NAME}" )

    set ( MACOSX_BUNDLE_BUNDLE_NAME ${TITLE} )
    set ( MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT}" )
    set ( MACOSX_BUNDLE_INFO_STRING "${INFO_STRING}" )
    set ( MACOSX_BUNDLE_GUI_IDENTIFIER ${TITLE} )

    set ( BUNDLE_FILES )

    foreach(durr ${BUNDLE_RSRCS})
        file(GLOB_RECURSE TMP ${durr}/* )
        foreach(file ${TMP})
            file ( RELATIVE_PATH file_dir ${durr} ${file} )
            get_filename_component ( file_dir "${file_dir}" DIRECTORY )
            get_filename_component ( file_name "${file_dir}" NAME )
            if(NOT ("${file_name}" STREQUAL ".DS_Store"))
                list ( APPEND BUNDLE_FILES ${file} )
                set_source_files_properties(
                    ${file} PROPERTIES MACOSX_PACKAGE_LOCATION
                    "${RESOURCE_DIR}/${file_dir}" )
            endif()
        endforeach()
    endforeach()

    add_executable(${TARGET} MACOSX_BUNDLE ${BUNDLE_FILES} ${OSX_ICON} ${SOURCES})

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        ${CMAKE_PACKAGED_OUTPUT_PREFIX}/osx
        )
endmacro()
