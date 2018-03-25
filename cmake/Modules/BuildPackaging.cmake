include ( AndroidApkBuild )
include ( LinuxAppImageBuild )
include ( LinuxFlatpakBuild )
include ( LinuxSnappyBuild )
include ( MacAppBuild )
include ( WindowsImageBuild )
include ( EmscriptenBuild )
include ( GamecubeBuild )

function(COFFEE_APPLICATION)

    #    TARGET
    #    TITLE APP_COMPANY_NAME APP_VERSION_CODE
    #    INFO_STRING COPYRIGHT
    #    SOURCES LIBRARIES
    #    BUNDLE_LIBS BUNDLE_RSRCS BUNDLE_LICENSES
    #    PERMISSIONS
    #    BUNDLE_BINARIES

    set ( APP_PACKAGE_PREFIX "me.birchtrees" )
    set ( APP_COMPANY "Birchtrees" )
    set ( APP_VERSION_CODE "1" )
    set ( APP_INFO_STRING "This is a Coffee application" )
    set ( APP_COPYRIGHT "MIT licensed code" )
    set ( APP_TITLE "Coffee Application" )
    set ( APP_PERMISSIONS
        OPENGL AUDIO ENVIRONMENT_SENSORS
        NETWORK_CONNECT JOYSTICK
        )

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
        )
    set ( switches
        # Whether to automatically include ASIO
        # Does not fail when ASIO is not built
        # This is used for automatic inclusion of profiler report code
        # Defines FEATURE_USE_ASIO when present
        USE_ASIO
        )

    cmake_parse_arguments( APP
        "${switches}" "${oneOpts}" "${multiOpts}"
        ${ARGN}
        )

    # These define and create LICENSE_FILE and APPLICATION_INFO_FILE
    coffee_gen_licenseinfo("${APP_TARGET}" "${APP_BUNDLE_LICENSES}")
    coffee_gen_applicationinfo("${APP_TARGET}"
        "${APP_TITLE}"
        "${APP_COMPANY_NAME}"
        "${APP_VERSION_CODE}"
        )

    set ( SOURCES_MOD
        "${APPLICATION_INFO_FILE};${LICENSE_FILE};${APP_SOURCES}"
        )

    set ( ICON_ASSET "${COFFEE_DESKTOP_DIRECTORY}/common/icon.svg" )

    if(ANDROID)
        ANDROIDAPK_PACKAGE(
            "${APP_TARGET}"
            "${APP_PACKAGE_PREFIX}"
            "${APP_TITLE}" "${APP_VERSION_CODE}"
            "${APP_COPYRIGHT}" "${APP_COMPANY}"
            "${SOURCES_MOD}"
            "${APP_RESOURCES}"
            "${APP_BUNDLE_LIBRARIES}"
            "${ICON_ASSET}" )
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
    elseif(APPLE)
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

        if(COFFEE_GENERATE_APPIMAGE)
            APPIMAGE_PACKAGE(
                ${APP_TARGET}
                "${APP_TITLE}"
                "${APP_RESOURCES}"
                ""
                "${APP_BUNDLE_LIBRARIES}"
                "${ICON_ASSET}" )
        endif()
        if(COFFEE_GENERATE_FLATPAK)
            FLATPAK_PACKAGE(
                ${APP_TARGET}
                "${APP_PACKAGE_PREFIX}" "${APP_TITLE}"
                "${APP_VERSION_CODE}"
                "${APP_COPYRIGHT}" "${APP_COMPANY}"
                "${APP_RESOURCES}"
                "" "${APP_BUNDLE_LIBRARIES}"
                "${ICON_ASSET}" )
        endif()
        if(COFFEE_GENERATE_SNAPPY)
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

    # A little convenience
    if(APP_USE_ASIO AND TARGET CoffeeASIO)
        target_compile_definitions( ${APP_TARGET}
            PRIVATE
            -DFEATURE_USE_ASIO
            )
        target_link_libraries ( ${APP_TARGET}
            PRIVATE
            CoffeeASIO
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
