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

    set_default ( APP_PACKAGE_PREFIX "dev.birchy" )
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
    coffee_gen_applicationinfo(
        "${APP_TARGET}"
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

            APK_TARGET ${NDK_MAX_PLATFORM_LEVEL}
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

        # TODO: Add use of Path key to desktop file

        file ( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${APP_TARGET}.desktop"
"[Desktop Entry]
Version=1.0
Type=Application
Name=${APP_TITLE}
Comment=${APP_TITLE}
Icon=coffee-${APP_TARGET}
Categories=Game;
Terminal=false
Exec=${APP_TARGET} %s
StartupNotify=true
StartupWMClass=${APP_TARGET}
"
            )

        set ( XDG_SCHEMES )

        foreach ( SCHEME ${APP_SCHEMES} )
            set ( XDG_SCHEMES "x-scheme-handler/${SCHEME}\;${XDG_SCHEMES}" )
        endforeach()

        if( NOT "${XDG_SCHEMES}" STREQUAL "" )
            file ( APPEND "${CMAKE_CURRENT_BINARY_DIR}/${APP_TARGET}.desktop"
                "MimeType=${XDG_SCHEMES}\n"
                )
        endif()

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
            TARGET "${APP_TARGET}"
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

    set_target_properties ( ${APP_TARGET} PROPERTIES
        APP_TITLE "${APP_TITLE}"
        APP_COLOR "#E91E63"
        APP_INFO_STRING "${APP_INFO_STRING}"
        APP_COPYRIGHT "${APP_COPYRIGHT}"
        APP_COMPANY "${APP_COMPANY}"
        APP_VERSION_INT "${APP_VERSION_CODE}"
        APP_VERSION_STR "${COFFEE_BUILD_STRING}"
        ICON_SOURCE "${ICON_ASSET}"
        PKG_NAME "${APP_PACKAGE_PREFIX}.${APP_TARGET}"
        IS_GAME "true"
        )

    target_compile_definitions ( ${APP_TARGET}
        PRIVATE
        -DCOFFEE_COMPONENT_NAME="${APP_TARGET}"
        -DCOFFEE_COMPILE_APPLICATION=1
        )

    target_enable_cxx11(${APP_TARGET})
    target_enable_lto(${APP_TARGET})

    set ( CORE_APP_LIB Coffee::CoreApplication )

    if("${PROJECT_NAME}" STREQUAL "Coffee")
        set ( CORE_APP_LIB CoreApplication )
    endif()

    target_link_libraries ( ${APP_TARGET}
        PUBLIC
        ${APP_LIBRARIES}
        ${CORE_APP_LIB}
        )

endfunction()
