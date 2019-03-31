if(ANDROID)
    if(BUILD_SDL2)
        message ( FATAL "Invalid configuration" )
    endif()

    include ( InkscapeResize )
    include ( PermissionList )

    # APK signing

    set ( ANDROID_APK_SIGN_KEY "~/keystore/key.release"
        CACHE FILEPATH "Android signing key" )

    set ( ANDROID_APK_SIGN_ALIAS "$ENV{ANDROID_SIGN_ALIAS}"
        CACHE STRING "Android signing key alias" )

    # Misc properties

    set ( ANDROID_BUILD_TOOLS_VER "28.0.2" CACHE STRING "" )

    set ( ANDROID_APK_OUTPUT_DIR "${COFFEE_PACKAGE_DIRECTORY}/android-apk"
        CACHE PATH "" )

    set ( ANDROID_PROJECT_INPUT "${COFFEE_DESKTOP_DIRECTORY}/android"
        CACHE STRING "" )

    set ( ANDROID_BUILD_OUTPUT "${COFFEE_DEPLOY_DIRECTORY}/android-apk"
        CACHE STRING "" )

    set ( GRADLE_ROOT "app/src/main" )
    set ( JAVA_SRC_PREFIX "${GRADLE_ROOT}/java" )
    set ( APK_RSC_PREFIX "${GRADLE_ROOT}/res" )
    set ( MANIFEST_PREFIX "${GRADLE_ROOT}" )
    set ( NATIVE_LIBS_PREFIX "${GRADLE_ROOT}/jniLibs" )
    set ( APK_ASSET_PREFIX "${GRADLE_ROOT}/assets" )
endif()

if(NOT TARGET AndroidPackage)
    add_custom_target ( AndroidPackage )
endif()

function(ANDROIDAPK_PACKAGE)

    set ( oneOpts
        TARGET

        DOM_NAME
        TITLE

        VERSION_CODE
        COPYRIGHT
        COMPANY

        APK_TARGET
        APK_MIN_TARGET
        )

    set ( multiOpts
        SOURCES

        RESOURCES
        BUNDLE_LIBRARIES

        )

    cmake_parse_arguments( AAPK "" "${oneOpts}" "${multiOpts}" ${ARGN} )

    ############################################################################
    ############################################################################
    ############################################################################

    add_library(${AAPK_TARGET} SHARED ${AAPK_SOURCES} )
    set_property(TARGET ${AAPK_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

    get_property (TARGET_LINK_FLAGS TARGET "${AAPK_TARGET}" PROPERTY LINK_FLAGS)

    set_property ( TARGET "${AAPK_TARGET}"
        PROPERTY LINK_FLAGS
        "${TARGET_LINK_FLAGS} -u ANativeActivity_onCreate"
        )

    add_custom_target ( "${AAPK_TARGET}.project" DEPENDS "${AAPK_TARGET}" )
    add_custom_target ( "${AAPK_TARGET}.apk" DEPENDS "${AAPK_TARGET}.project" )

    add_dependencies( AndroidPackage "${AAPK_TARGET}.apk" )

    # Lowercase the target name for package name
    string ( TOLOWER "${AAPK_TARGET}" PACKAGE_SUFFIX )

    ############################################################################
    ############################################################################
    ############################################################################

    set ( DEPENDENCIES )

    list ( APPEND DEPENDENCIES "$<TARGET_FILE:${AAPK_TARGET}>" )

    list ( APPEND DEPENDENCIES "${AAPK_BUNDLE_LIBRARIES}" )

    ############################################################################
    ############################################################################
    ############################################################################

    #
    # AndroidManifest.xml templating
    #

        set ( ANDROID_APPLICATION_COLOR "#E91E63" )
        set ( ANDROID_APK_BANNER_DATA "" )
        set ( ANDROID_APK_LOGO_DATA "" )
        set ( ANDROID_APPLICATION_NAME "${AAPK_TITLE}" )
        set ( ANDROID_VERSION_CODE ${AAPK_VERSION_CODE} )
        set ( ANDROID_VERSION_NAME ${COFFEE_BUILD_STRING} )

        set ( ANDROID_API_TARGET ${AAPK_APK_TARGET} )
        set ( ANDROID_API_MIN_TARGET ${AAPK_APK_MIN_TARGET} )

        set ( ANDROID_APK_IS_GAME "android:isGame=\"true\"" )

        set ( ANDROID_PACKAGE_NAME ${AAPK_DOM_NAME}.${PACKAGE_SUFFIX} )
        set ( ANDROID_STARTUP_LIBRARY "${AAPK_TARGET}" )
        set ( ANDROID_ACTIVITY_NAME "${ANDROID_PACKAGE_NAME}.NativeActivity" )
        set ( ANDROID_APK_NAME "${ANDROID_PACKAGE_NAME}-release-unsigned.apk" )

        #########################################################
        #
        # Android features
        #
        #########################################################
        set ( ANDROID_FEATURES "" )

        set ( ANDROID_REQUIRED_FEATURES
            "android.hardware.faketouch"
            "android.hardware.gamepad"
            "android.hardware.screen.landscape"
            "android.hardware.audio.output"
            )

        set ( ANDROID_OPTIONAL_FEATURES
            "android.hardware.usb.host"
            "android.hardware.touchscreen"
            "android.hardware.touchscreen.multitouch"
            "android.hardware.touchscreen.multitouch.distinct"
            "android.hardware.touchscreen.multitouch.jazzhand"
            "android.hardware.sensor.accelerometer"
            "android.hardware.sensor.gyroscope"
            "android.hardware.sensor.hifi_sensors"
            "android.hardware.opengles.aep"
            "${ANDROID_CUSTOM_PERMISSIONS}"
            )

        foreach(feat ${ANDROID_REQUIRED_FEATURES})
            set ( ANDROID_FEATURES
                "${ANDROID_FEATURES}
        <uses-feature android:name=\"${feat}\"
            android:required=\"true\" />")
        endforeach()
        foreach(feat ${ANDROID_OPTIONAL_FEATURES})
            set ( ANDROID_FEATURES
                "${ANDROID_FEATURES}
        <uses-feature android:name=\"${feat}\"
            android:required=\"false\" />")
        endforeach()

        if(BUILD_GLES)
            set ( ANDROID_FEATURES
                "${ANDROID_FEATURES}

        <uses-feature android:glEsVersion=\"0x00020000\"
            android:required=\"true\" />"
                )

            set ( ANDROID_FEATURES
                "${ANDROID_FEATURES}
        <uses-feature android:glEsVersion=\"0x00030000\"
            android:required=\"false\" />
        <uses-feature android:glEsVersion=\"0x00030001\"
            android:required=\"false\" />
        <uses-feature android:glEsVersion=\"0x00030002\"
            android:required=\"false\" />"
                )
        endif()

        #########################################################
        #
        # APK permissions
        #
        #########################################################

        set ( ANDROID_PERMISSIONS_PRE
            "android.permission.INTERNET"
            "android.permission.VIBRATE")

        foreach(perm ${ANDROID_PERMISSIONS_PRE})
            set ( ANDROID_PERMISSIONS
                "${ANDROID_PERMISSIONS}
        <uses-permission android:name=\"${perm}\" />"
                )
        endforeach()

        # For valid options, see:
        # http://developer.android.com/guide/topics/manifest/activity-element.html
        set ( ANDROID_ORIENTATION_MODE "sensorLandscape" )

    ############################################################################
    ############################################################################
    ############################################################################


    #
    # APK naming
    #

        # For now, manage release builds through Android Studio
        set ( RELEASE_PREFIX "debug" )

        set ( ANDROID_APK_FILE_OUTPUT
            "${ANDROID_APK_OUTPUT_DIR}/${ANDROID_PACKAGE_NAME}_${RELEASE_PREFIX}.apk" )

        set( BUILD_OUTDIR ${ANDROID_BUILD_OUTPUT}/${AAPK_TARGET} )

        string ( REGEX REPLACE
            "([a-zA-Z0-9_-]+)\ .*" "\\1"
            LIB_SUFFIX "${ANDROID_ABI}"  )

        set ( ANDROID_LIB_OUTPUT_DIRECTORY
            "${BUILD_OUTDIR}/${NATIVE_LIBS_PREFIX}/${LIB_SUFFIX}" )
        set ( ANDROID_ASSET_OUTPUT_DIRECTORY
            "${BUILD_OUTDIR}/${APK_ASSET_PREFIX}" )

    ############################################################################
    ############################################################################
    ############################################################################

    #
    # Configure gradle/Android project files
    #

        #
        # Create build directory at configure time
        #
        execute_process (
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ANDROID_PROJECT_INPUT}/Gradle" "${BUILD_OUTDIR}"
            )

        #
        # We need to change the Gradle version used by the project
        #
        set ( GRADLE_WRAPPER_FILE
            "${BUILD_OUTDIR}/gradle/wrapper/gradle-wrapper.properties" )
        file ( READ "${GRADLE_WRAPPER_FILE}" GRADLE_PROPERTIES )
        string ( REGEX REPLACE "gradle-[0-9\.]+-all" "gradle-4.10-all"
            GRADLE_PROPERTIES "${GRADLE_PROPERTIES}" )
        file( WRITE "${GRADLE_WRAPPER_FILE}"  "${GRADLE_PROPERTIES}" )

        add_custom_command( TARGET "${AAPK_TARGET}.project" POST_BUILD
            COMMAND chmod +x ${BUILD_OUTDIR}/gradlew
            )

        # Fill in AndroidManifest with info create above
        configure_file (
            "${ANDROID_PROJECT_INPUT}/AndroidManifest.xml.in"
            "${BUILD_OUTDIR}/${MANIFEST_PREFIX}/AndroidManifest.xml"
            @ONLY
            )
        # Configure strings.xml with app info
        configure_file (
            "${ANDROID_PROJECT_INPUT}/strings.xml.in"
            "${BUILD_OUTDIR}/${APK_RSC_PREFIX}/values/strings.xml"
            @ONLY
            )
        file ( WRITE "${BUILD_OUTDIR}/local.properties"
            "sdk.dir=${ANDROID_SDK}\n"
            "ndk.dir=${ANDROID_NDK}")

        configure_file (
            "${ANDROID_PROJECT_INPUT}/build.gradle.in"
            "${BUILD_OUTDIR}/app/build.gradle"
            )

        #
        # Create project directories
        #
        add_custom_command ( TARGET "${AAPK_TARGET}.project"
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E
            make_directory "${ANDROID_ASSET_OUTPUT_DIRECTORY}" )

        add_custom_command ( TARGET "${AAPK_TARGET}.project"
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E
            make_directory "${ANDROID_APK_OUTPUT_DIR}" )

        add_custom_command ( TARGET "${AAPK_TARGET}.project"
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E
            make_directory "${ANDROID_LIB_OUTPUT_DIRECTORY}" )

        #
        # Install asset files
        #
        foreach (resc ${AAPK_RESOURCES})
            add_custom_command ( TARGET "${AAPK_TARGET}.project"
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E
                copy_directory "${resc}" .
                WORKING_DIRECTORY "${ANDROID_ASSET_OUTPUT_DIRECTORY}" )
        endforeach()

        #
        # Install dependency libraries
        #
        foreach(lib ${DEPENDENCIES})
            add_custom_command ( TARGET "${AAPK_TARGET}.project"
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E
                copy "${lib}" "${ANDROID_LIB_OUTPUT_DIRECTORY}"
                )
        endforeach()

    ############################################################################
    ############################################################################
    ############################################################################

    #
    # Build steps
    #

        add_custom_command( TARGET "${AAPK_TARGET}.apk"
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env
            GRADLE_OPTS="-Dorg.gradle.daemon=false"

            ${BUILD_OUTDIR}/gradlew assembleDebug

            WORKING_DIRECTORY ${BUILD_OUTDIR}
            )
        add_custom_command ( TARGET "${AAPK_TARGET}.apk"
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${BUILD_OUTDIR}/app/build/outputs/apk/${RELEASE_PREFIX}/app-${RELEASE_PREFIX}.apk"
            "${ANDROID_APK_FILE_OUTPUT}"
            )

    ############################################################################
    ############################################################################
    ############################################################################

    install (
        FILES
        ${ANDROID_APK_FILE_OUTPUT}

        DESTINATION
        ${CMAKE_PACKAGED_OUTPUT_PREFIX}/android-apk
        )
endfunction()
