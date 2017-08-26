include ( AndroidApkBuild )
include ( LinuxAppImageBuild )
include ( LinuxFlatpakBuild )
include ( LinuxSnappyBuild )
include ( MacAppBuild )
include ( WindowsImageBuild )

# TODO: Make package name more configurable
# Android only uses shared libraries which are loaded, all else uses typical executables
# We do a test to check if a library is a shared library for Android
# For iOS, everything will be statically linked, which might be used for Android as well.
# For now, we leave the linking options here for desktop platforms
function(COFFEE_ADD_APPLICATION_LONGERER
        TARGET
        TITLE APP_COMPANY_NAME APP_VERSION_CODE
        INFO_STRING COPYRIGHT
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS BUNDLE_LICENSES
        PERMISSIONS
        BUNDLE_BINARIES)

    coffee_gen_licenseinfo("${TARGET}" "${BUNDLE_LICENSES}")
    coffee_gen_applicationinfo("${TARGET}"
        "${TITLE}"
        "${APP_COMPANY_NAME}"
        "${APP_VERSION_CODE}"
        )

    set ( SOURCES_MOD "${APPLICATION_INFO_FILE};${LICENSE_FILE};${SOURCES}" )

    set ( COMPANY "${APP_COMPANY_NAME}" )
    set ( VERSION_CODE "${APP_VERSION_CODE}" )

    set ( PACKAGE_PREFIX "org.coffee" )

    set ( ICON_ASSET "${COFFEE_DESKTOP_DIRECTORY}/common/icon.svg" )

    if(ANDROID)
        ANDROIDAPK_PACKAGE(
            "${TARGET}"
            "${PACKAGE_PREFIX}" "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${SOURCES_MOD}"
            "${BUNDLE_RSRCS}"
            "${BUNDLE_LIBS}"
            "${ICON_ASSET}" )
    elseif(WIN32)
        WINPE_PACKAGE(
            ${TARGET}
            "${PACKAGE_PREFIX}" "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${INFO_STRING}"
            "${SOURCES_MOD}"
            "${BUNDLE_RSRCS}"
            "${ICON_ASSET}")
    elseif(APPLE)
        MACAPP_PACKAGE(
            "${TARGET}"
            "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${INFO_STRING}"
            "${SOURCES_MOD}"
            "${BUNDLE_RSRCS}"
            "${BUNDLE_LIBS}"
            "${ICON_ASSET}" )
    elseif(EMSCRIPTEN)
        if(COFFEE_GENERATE_HTML)
            set ( CMAKE_EXECUTABLE_SUFFIX ".html" )
        endif()
        add_executable(${TARGET} ${SOURCES_MOD})
        if(NOT COFFEE_GENERATE_HTML)
            install(
                TARGETS
                ${TARGET}

                DESTINATION
                bin
                )
        else()
            set_target_properties( ${TARGET} PROPERTIES
              RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle
            )
            install(
                DIRECTORY
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle

                DESTINATION
                bin
                )
        endif()
    elseif(NACL)
        include_directories( ${SDL2_INCLUDE_DIR} )

        add_executable(${TARGET}
            ${SDL2_MAIN_C_FILE}
            ${SOURCES_MOD}
            )

        set ( OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle" )

        execute_process( COMMAND "${CMAKE_COMMAND}" -E make_directory "${OUT_DIR}" )

        # Create a manifest
        configure_file(
            "${COFFEE_DESKTOP_DIRECTORY}/native-client/application.nmf.in"
            "${OUT_DIR}/${TARGET}.nmf"
            )

        # Create a simple, testable HTML page
        configure_file(
            "${COFFEE_DESKTOP_DIRECTORY}/native-client/index.html.in"
            "${OUT_DIR}/index.html"
            )

        set_target_properties( ${TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${OUT_DIR}")

        add_custom_command( TARGET ${TARGET}
            POST_BUILD
            COMMAND ${NACL_FINALIZE} $<TARGET_FILE:${TARGET}>
            )

        install(
            DIRECTORY
            ${OUT_DIR}

            DESTINATION
            bin
            )
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        add_executable( ${TARGET} ${SOURCES_MOD} )

        # We still install a basic binary executable
        install(
            FILES
            "$<TARGET_FILE:${TARGET}>"

            DESTINATION
            "bin/${CMAKE_LIBRARY_ARCHITECTURE}"
            )

        if(COFFEE_GENERATE_APPIMAGE)
            APPIMAGE_PACKAGE(
                ${TARGET}
                "${TITLE}"
                "${BUNDLE_RSRCS}" "" "${BUNDLE_LIBS}"
                "${ICON_ASSET}" )
        endif()
        if(COFFEE_GENERATE_FLATPAK)
            FLATPAK_PACKAGE(
                ${TARGET}
                "${PACKAGE_PREFIX}" "${TITLE}" "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
                "${BUNDLE_RSRCS}"
                "" "${BUNDLE_LIBS}"
                "${ICON_ASSET}" )
        endif()
        if(COFFEE_GENERATE_SNAPPY)
            SNAPPY_PACKAGE(
                ${TARGET}
                "${TITLE}" "${INFO_STRING}"
                "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
                "${BUNDLE_RSRCS}"
                "" "${BUNDLE_LIBS}"
                "${BUNDLE_BINARIES}"
                "${ICON_ASSET}"
                "${PERMISSIONS}"
                )
        endif()
    else()

        add_executable(${TARGET} ${SOURCES_MOD})

        install(
            TARGETS
            ${TARGET}

            DESTINATION
            "bin/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    endif()


    target_enable_cxx11(${TARGET})

    target_link_libraries ( ${TARGET}
        PUBLIC
        ${LIBRARIES}
        )

    if(NACL)
        target_link_libraries( ${TARGET}
            PUBLIC
            ${SDL2_LIBRARY}
            ${SDL2_LIBRARIES}
            ppapi_gles2
            )
    endif()

    remove_definitions(
        -DCOFFEE_APPLICATION_NAME -DCOFFEE_ORGANIZATION_NAME
        -DCOFFEE_VERSION_CODE )

endfunction()

# Wrapper functions

macro(COFFEE_ADD_APPLICATION_LONGER
        TARGET
        TITLE COMPANY VERSION
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS)

    set ( PERMISSIONS "OPENGL;AUDIO;ENVIRONMENT_SENSORS;NETWORK_CONNECT;JOYSTICK" )
    set ( INFO_STRING "It's a Coffee application" )
    set ( COPYRIGHT "Copyright hbirchtree under MIT license" )

    COFFEE_ADD_APPLICATION_LONGERER(
        "${TARGET}"
        "${TITLE}" "${COMPANY}" "${VERSION}"
        "${INFO_STRING}" "${COPYRIGHT}"
        "${SOURCES}" "${LIBRARIES}"
        "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}" "${BUNDLE_LICENSES}"
        "${PERMISSIONS}"
        "")
endmacro()

function(COFFEE_ADD_EXAMPLE_LONGER
        TARGET
        TITLE APP_COMPANY_NAME APP_VERSION_CODE
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS BUNDLE_LICENSES)
    COFFEE_ADD_APPLICATION_LONGER(
        "${TARGET}"
        "${TITLE}" "${COMPANY}" "${VERSION}"
        "${SOURCES}" "${LIBRARIES}"
        "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}" "${BUNDLE_LICENSES}")
endfunction()

macro(COFFEE_ADD_EXAMPLE_LONG
        TARGET
        TITLE
        SOURCES LIBRARIES
        BUNDLE_LIBS BUNDLE_RSRCS)

    set ( VERSION 5 )
    set ( COMPANY "Coffee Guy" )

    COFFEE_ADD_EXAMPLE_LONGER(
        "${TARGET}"
        "${TITLE}" "${COMPANY}" "${VERSION}"
        "${SOURCES}" "${LIBRARIES}"
        "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}" "${BUNDLE_LICENSES}")
endmacro()

macro(COFFEE_ADD_APPLICATION_LONG TARGET TITLE SOURCES LIBRARIES BUNDLE_LIBS BUNDLE_RSRCS)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "${BUNDLE_LIBS}" "${BUNDLE_RSRCS}")
endmacro()

macro(COFFEE_ADD_EXAMPLE TARGET TITLE SOURCES LIBRARIES)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "" "")
endmacro()

macro(COFFEE_ADD_APPLICATION TARGET TITLE SOURCES LIBRARIES)
    coffee_add_example_long(${TARGET} ${TITLE} "${SOURCES}" "${LIBRARIES}" "" "")
endmacro()
