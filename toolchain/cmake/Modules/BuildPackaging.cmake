include ( AndroidApkBuild )
include ( LinuxAppImageBuild )
include ( LinuxFlatpakBuild )
include ( LinuxSnappyBuild )
include ( MacAppBuild )
include ( WindowsImageBuild )
include ( EmscriptenBuild )
include ( GamecubeBuild )

macro ( SET_DEFAULT VAR_NAME )
    if(NOT DEFINED ${VAR_NAME})
        set ( ${VAR_NAME} ${ARGN} )
    endif()
endmacro()

function(COFFEE_APPLICATION)

    #    TARGET
    #    TITLE APP_COMPANY_NAME APP_VERSION_CODE
    #    INFO_STRING COPYRIGHT
    #    SOURCES LIBRARIES
    #    BUNDLE_LIBS BUNDLE_RSRCS BUNDLE_LICENSES
    #    PERMISSIONS
    #    BUNDLE_BINARIES

    set ( oneOpts
        TARGET
        # Aesthetic details
        TITLE
        COMPANY
        VERSION_CODE
        INFO_STRING
        COPYRIGHT

        # Package name generation
        # PACKAGE_PREFIX is prepended to target name
        PACKAGE_PREFIX
        # Defines entire package name
        PACKAGE_NAME
        )
    set ( multiOpts
        # Application permissions, translated per platform
        PERMISSIONS

        # Linking data
        SOURCES
        LIBRARIES

        # For shared libraries
        BUNDLE_LIBRARIES

        # Just resources used by the program
        # Specified as the root of a resource directory
        RESOURCES
        # For required programs
        BUNDLE_BINARIES
        # For licenses, will be merged into binary
        BUNDLE_LICENSES

        # Protocol schemes to register the application with
        SCHEMES
        )
    set ( switches
        # Indicates building a console application. Mostly used on OSX to
        #  disable building as .app. Will output plain binary on OSX.
        USE_CMD
        )

    cmake_parse_arguments( APP
        "${switches}" "${oneOpts}" "${multiOpts}"
        ${ARGN}
        )

    set_default ( APP_PACKAGE_PREFIX "me.birchtrees" )
    set_default ( APP_COMPANY "Birchtrees" )
    set_default ( APP_VERSION_CODE "1" )
    set_default ( APP_INFO_STRING "This is a Coffee application" )
    set_default ( APP_COPYRIGHT "MIT licensed code" )
    set_default ( APP_TITLE "Coffee Application" )
    set_default ( APP_PERMISSIONS
        OPENGL AUDIO ENVIRONMENT_SENSORS
        NETWORK_CONNECT JOYSTICK
        )

    # These define and create LICENSE_FILE and APPLICATION_INFO_FILE
    coffee_gen_licenseinfo("${APP_TARGET}" "${APP_BUNDLE_LICENSES}")
    coffee_gen_applicationinfo("${APP_TARGET}"
        "${APP_TITLE}"
        "${APP_COMPANY}"
        "${APP_VERSION_CODE}"
        )

    set ( SOURCES_MOD
        "${APPLICATION_INFO_FILE};${LICENSE_FILE};${APP_SOURCES}"
        )

    set ( ICON_ASSET "${COFFEE_DESKTOP_DIRECTORY}/common/icon.svg" )

    if(ANDROID)
        ANDROIDAPK_PACKAGE(
            TARGET "${APP_TARGET}"

            TITLE "${APP_TITLE}"
            DOM_NAME "${APP_PACKAGE_PREFIX}"
            VERSION_CODE "${APP_VERSION_CODE}"
            COPYRIGHT "${APP_COPYRIGHT}"
            COMPANY "${APP_COMPANY}"

            SOURCES ${SOURCES_MOD}
            RESOURCES ${APP_RESOURCES}
            BUNDLE_LIBRARIES ${APP_BUNDLE_LIBRARIES}

            APK_TARGET 27
            APK_MIN_TARGET ${ANDROID_NATIVE_API_LEVEL}
            )
    elseif(WIN32)
        WINPE_PACKAGE(
            ${APP_TARGET}
            "${APP_PACKAGE_PREFIX}"
            "${APP_TITLE}" "${APP_VERSION_CODE}"
            "${APP_COPYRIGHT}" "${APP_COMPANY}"
            "${APP_INFO_STRING}"
            "${SOURCES_MOD}"
            "${APP_RESOURCES}"
            "${ICON_ASSET}")
    elseif(APPLE AND NOT APP_USE_CMD)
        MACAPP_PACKAGE(
            "${APP_TARGET}"
            "${APP_TITLE}" "${APP_VERSION_CODE}"
            "${APP_COPYRIGHT}" "${APP_COMPANY}"
            "${APP_INFO_STRING}"
            "${SOURCES_MOD}"
            "${APP_RESOURCES}"
            "${APP_BUNDLE_LIBRARIES}"
            "${ICON_ASSET}" )
    elseif(EMSCRIPTEN)
        EMSCRIPTEN_PACKAGE(
            TARGET ${APP_TARGET}

            SOURCES ${SOURCES_MOD}
            RESOURCES ${APP_RESOURCES}
            ICON ${ICON_TARGET}
            )
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        add_executable( ${APP_TARGET} ${SOURCES_MOD} )

        # We still install a basic binary executable
        install(
            FILES
            "$<TARGET_FILE:${APP_TARGET}>"

            DESTINATION
            "bin/${CMAKE_LIBRARY_ARCHITECTURE}"
            )

        if(GENERATE_APPIMAGE)
            APPIMAGE_PACKAGE(
                ${APP_TARGET}
                "${APP_TITLE}"
                "${APP_RESOURCES}"
                ""
                "${APP_BUNDLE_LIBRARIES}"
                "${ICON_ASSET}" )
        endif()
        if(GENERATE_FLATPAK)
            FLATPAK_PACKAGE(
                ${APP_TARGET}
                "${APP_PACKAGE_PREFIX}" "${APP_TITLE}"
                "${APP_VERSION_CODE}"
                "${APP_COPYRIGHT}" "${APP_COMPANY}"
                "${APP_RESOURCES}"
                "" "${APP_BUNDLE_LIBRARIES}"
                "${ICON_ASSET}" )
        endif()
        if(GENERATE_SNAPPY)
            SNAPPY_PACKAGE(
                ${APP_TARGET}
                "${APP_TITLE}" "${APP_INFO_STRING}"
                "${APP_VERSION_CODE}" "${APP_COPYRIGHT}"
                "${APP_COMPANY}"
                "${APP_RESOURCES}"
                "" "${APP_BUNDLE_LIBRARIES}"
                "${APP_BUNDLE_BINARIES}"
                "${ICON_ASSET}"
                "${APP_PERMISSIONS}"
                )
        endif()
    elseif(GAMECUBE OR WII)
        GAMECUBE_PACKAGE(
            TARGET ${APP_TARGET}
            SOURCES ${SOURCES_MOD}
            )
    else()
        add_executable(${APP_TARGET} ${SOURCES_MOD})

        install(
            TARGETS
            ${APP_TARGET}

            DESTINATION
            "bin/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    endif()

    target_compile_definitions ( ${APP_TARGET}
        PRIVATE
        -DCOFFEE_COMPONENT_NAME="${APP_TARGET}"
        )

    target_enable_cxx11(${APP_TARGET})

    target_link_libraries ( ${APP_TARGET}
        PUBLIC
        ${APP_LIBRARIES}
        CoffeeCore_Application
        )

endfunction()
