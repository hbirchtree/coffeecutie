if(APPLE)
    set ( OSX_DEFAULT_ICON "${COFFEE_DESKTOP_DIRECTORY}/osx/Coffee.icns" CACHE FILEPATH "" )
endif()

macro( MACFRAMEWORK_PACKAGE
        TARGET LINKOPT
        VERSION_CODE COPYRIGHT COMPANY
        SOURCES BUNDLE_RSRCS BUNDLE_HEADERS)

    set ( VERSION_STD_CODE "1.0.0" )
    set ( VERSION_COMP_CODE "1.0.0" )

    add_definitions( -DCOFFEE_APPLICATION_LIBRARY )

    if(NOT IOS)
        add_library("${TARGET}" ${LINKOPT} "${SOURCES}")
    else()
        add_library("${TARGET}" STATIC "${SOURCES}")
    endif()

    set_target_properties("${TARGET}" PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER org.coffeecutie-dev.${TARGET}
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${VERSION_CODE}
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${VERSION_CODE}
        VERSION ${VERSION_STD_CODE}
        SOVERSION ${VERSION_COMP_CODE}
        )

    # TODO: Process resource files
    # TODO: Process header files

    install(
        TARGETS
        "${TARGET}"

        LIBRARY DESTINATION
        lib

        ARCHIVE DESTINATION
        lib

        FRAMEWORK DESTINATION
        frameworks
        COMPONENT
        bin
        )
endmacro()

macro( MACAPP_PACKAGE
        TARGET
        TITLE VERSION_CODE COPYRIGHT COMPANY
        INFO_STRING
        SOURCES BUNDLE_RSRCS BUNDLE_LIBS
        OSX_ICON )

    # iOS stores resources at the bottom level
    if(IOS)
        set ( RESOURCE_DIR "" )
    else()
        set ( RESOURCE_DIR "Resources" )
    endif()

    set ( FRAMEWORK_DIR "Frameworks" )

    # For installing the icon in the bundle
    set_source_files_properties ( ${OSX_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "${RESOURCE_DIR}" )
    # Extracted in order to be inserted into Info.plist
    get_filename_component ( OSX_ICON_NAME "${OSX_ICON}" NAME )

    # An exhaustive list of all the resources that will be bundled
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

    # Installing frameworks
    foreach(fw ${BUNDLE_LIBS})
        message ( "Framework: ${fw}" )
    endforeach()

    #    if(NOT IOS)
    add_executable(${TARGET} MACOSX_BUNDLE
        ${BUNDLE_FILES} ${OSX_ICON}
        ${SOURCES}
        )

    if(IOS)
        set ( IOS_NAME "${TITLE}" )
        set ( IOS_IDENTIFIER "${TITLE}" )
        set ( IOS_INFO "${INFO_STRING}" )

        set ( IOS_BUNDLEVER "${COFFEE_BUILD_STRING}" )
        set ( IOS_SHORTVER "${COFFEE_VERSION_CODE}" )
        set ( IOS_LONGVER "${COFFEE_BUILD_STRING}" )

        set ( IOS_EXEC "${TITLE}" )
        set ( IOS_LANG "English" )
        set ( IOS_LAUNCH_IMG "Default")
        set ( IOS_HIDE_STATUSBAR "true" )
        set ( IOS_PRERENDER_ICON "true" )

        set ( IOS_ORIENTATION "UIInterfaceOrientationLandscapeLeft" )

        set ( IOS_SIGNATURE "?" )

        set ( IOS_PLIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_Info.plist.in" )

        configure_file (
            "${COFFEE_DESKTOP_DIRECTORY}/osx/ios/Info.plist.xml"
            "${IOS_PLIST_FILE}"
            )

        set_target_properties ( ${TARGET} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${IOS_PLIST_FILE}"
            )
    endif()

    # Lots of properties!
    set_target_properties(${TARGET} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "${TITLE}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${TITLE}"
        MACOSX_BUNDLE_ICON_FILE "${OSX_ICON_NAME}"

        MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT}"
        MACOSX_BUNDLE_INFO_STRING "${INFO_STRING}"

        MACOSX_BUNDLE_BUNDLE_VERSION "${COFFEE_BUILD_STRING}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${COFFEE_VERSION_CODE}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${COFFEE_BUILD_STRING}"
        )

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        ${CMAKE_PACKAGED_OUTPUT_PREFIX}/apple-osx
        )
    #    else()
    #        set ( APP_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TITLE}.app" )



    #        add_executable ( ${TARGET} ${SOURCES} )

    #        add_custom_command (
    #            TARGET "${TARGET}"
    #            POST_BUILD
    #            COMMAND "${CMAKE_COMMAND}" -E make_directory "${APP_DIR}"
    #            )
    #        add_custom_command (
    #            TARGET "${TARGET}"
    #            POST_BUILD
    #            COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${TARGET}>" "${APP_DIR}/${TITLE}"
    #            )
    #        configure_file (
    #            "${COFFEE_DESKTOP_DIRECTORY}/osx/ios/Info.plist.xml"
    #            "${APP_DIR}/Info.plist"
    #            @ONLY
    #            )
    #    endif()
endmacro()