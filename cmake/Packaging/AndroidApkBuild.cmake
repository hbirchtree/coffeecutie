if(ANDROID)
    find_package ( CfAndroidMain )
    find_package ( AndroidToolkit )

    include ( AndroidNdkGdb )
    include ( InkscapeResize )

    if(ANDROID_USE_SDL2_LAUNCH)
        find_package(SDL2main REQUIRED)
    endif()

    # APK signing

    set ( ANDROID_APK_SIGN_KEY "~/keystore/key.release" CACHE FILEPATH "Android signing key" )

    set ( ANDROID_APK_SIGN_ALIAS "$ENV{ANDROID_SIGN_ALIAS}" CACHE STRING "Android signing key alias" )

    # Misc properties

    set ( ANDROID_APK_OUTPUT_DIR "${COFFEE_PACKAGE_DIRECTORY}/android-apk" CACHE PATH "" )

    set ( ANDROID_PROJECT_INPUT "${COFFEE_DESKTOP_DIRECTORY}/android" )

    set ( ANDROID_PROJECT_TEMPLATE_DIR "${ANDROID_PROJECT_INPUT}/Template" )
    set ( ANDROID_PROJECT_CONFIG_DIR "${ANDROID_PROJECT_INPUT}/Config" )

    set ( ANDROID_BUILD_OUTPUT "${COFFEE_DEPLOY_DIRECTORY}/android-apk" )

    message ( "-- Main File: ${SDL2_ANDROID_MAIN_FILE}" )

endif()

#
# Brief: Auto-generate an APK build of a given build target
#
# Target : which target to package
# Target_Name : target name
# App_Name : Name of application
# Pkg_Name : Name of Android package
# Version_Int : version code
# Version_Str : version string
# Api_Target : API target, also minimal API level
# Api_Arch : armeabi-v7a, arm64-v8a or x86
# Dependency_Libs : libraries which will be added to the APK
#
macro(APK_PACKAGE_EXT Target_Name App_Name Pkg_Name Version_Int Version_Str Api_Target Api_Arch Dependency_Libs Icon_File )

    message ( "-- Generating ${Pkg_Name} (${Api_Arch})" )

    set ( ANDROID_PACKAGE_NAME ${Pkg_Name} )

    # For SDL2-enabled programs
    if(ANDROID_USE_SDL2_LAUNCH)
        set ( ANDROID_STARTUP_ACTIVITY "CoffeeActivity" )
    else()
        set ( ANDROID_STARTUP_ACTIVITY "NativeActivity" )
    endif()

    # For NativeActivity programs
    set ( ANDROID_STARTUP_LIBRARY "${Target_Name}" )

    set ( ANDROID_ACTIVITY_NAME "${Pkg_Name}.${ANDROID_STARTUP_ACTIVITY}" )

    set ( ANDROID_APK_NAME "${Pkg_Name}.${ANDROID_STARTUP_ACTIVITY}-release-unsigned.apk" )

    set ( ANDROID_APPLICATION_NAME ${App_Name} )

    set ( ANDROID_VERSION_CODE ${Version_Int} )
    set ( ANDROID_VERSION_NAME ${Version_Str} )

    set ( ANDROID_API_TARGET ${Api_Target} )
    set ( ANDROID_API_MIN_TARGET "9" )


    # For valid options, see:
    # http://developer.android.com/guide/topics/manifest/activity-element.html
    set ( ANDROID_ORIENTATION_MODE "sensorLandscape" )

    set ( RELEASE_PREFIX )

    if( CMAKE_BUILD_TYPE STREQUAL "Release" )
        set ( RELEASE_PREFIX "rel" )
        set ( ANDROID_APK_DEBUGGABLE "false" )
    else()
        set ( RELEASE_PREFIX "dbg" )
        set ( ANDROID_APK_DEBUGGABLE "true" )
    endif()

    set ( ANDROID_APK_FILE_OUTPUT
        "${ANDROID_APK_OUTPUT_DIR}/${ANDROID_PACKAGE_NAME}_${RELEASE_PREFIX}.apk" )


    set( BUILD_OUTDIR ${ANDROID_BUILD_OUTPUT}/${Target_Name} )

    string ( REGEX REPLACE "([a-zA-Z0-9\_\-]+)\ .*" "\\1" LIB_SUFFIX "${Api_Arch}"  )

    set ( ANDROID_LIB_OUTPUT_DIRECTORY ${BUILD_OUTDIR}/libs/${LIB_SUFFIX} )
    set ( ANDROID_ASSET_OUTPUT_DIRECTORY ${BUILD_OUTDIR}/assets )

    set ( ANDROID_ANT_COMMON_PROPERTIES -Dout.final.file="${ANDROID_APK_FILE_OUTPUT}" )

    #
    # Debugging/deployment options
    #
    if(ANDROID_USE_SDL2_LAUNCH)
        set ( ANDROID_AM_START_INTENT "${ANDROID_PACKAGE_NAME}" )
        set ( ANDROID_AM_START_ACTIVITY "${ANDROID_PACKAGE_NAME}/.${ANDROID_STARTUP_ACTIVITY}" )
    else()
        set ( ANDROID_AM_START_INTENT "android.intent.action.LAUNCHER" )
        set ( ANDROID_AM_START_ACTIVITY "${ANDROID_PACKAGE_NAME}/android.app.NativeActivity" )
    endif()

    # Where the primary class is found, also decides names of package
    string ( REGEX REPLACE "\\." "/" ANDROID_MAIN_PATH "${ANDROID_PACKAGE_NAME}" )

    #
    # Create build directory
    #
    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_OUTDIR}
        )
    #
    # Mipmap the icons
    #
    set ( ANDROID_MIPMAP_SET
        "mdpi/48"
        "hdpi/72"
        "xhdpi/96"
        "xxhdpi/144"
        )

    #message ( "-- Mipmapping file ${ICON_ASSET} to ${ANDROID_MIPMAP_SET}" )

    foreach(mip ${ANDROID_MIPMAP_SET})
        string ( REGEX REPLACE "([a-z]+)/([0-9]+)" "\\1" MIPMAP_DEST_DIR "${mip}" )
        string ( REGEX REPLACE "([a-z]+)/([0-9]+)" "\\2" MIPMAP_SIZE "${mip}" )
        set ( MIPMAP_DEST_FILE "${BUILD_OUTDIR}/res/drawable-${MIPMAP_DEST_DIR}/ic_launcher.png" )
        add_custom_command ( TARGET ${Target_Name}
            PRE_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E
            make_directory "${BUILD_OUTDIR}/res/drawable-${MIPMAP_DEST_DIR}"
            )
        if(INKSCAPE_PROGRAM AND NOWAYFUCKOFF)
            inkscape_resize_svg( "${Target_Name}" "${ICON_ASSET}" ${MIPMAP_SIZE} "${MIPMAP_DEST_FILE}" )
        else()
            magick_resize_svg ( "${Target_Name}" "${ICON_ASSET}" "${MIPMAP_SIZE}" "${MIPMAP_DEST_FILE}" )
        endif()
    endforeach()

    #
    # Install base template files
    #
    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ANDROID_PROJECT_TEMPLATE_DIR} ${BUILD_OUTDIR}
        )
    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_LIB_OUTPUT_DIRECTORY}
        )

    #
    # Create library directory
    #
    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ANDROID_LIB_OUTPUT_DIRECTORY}"
        )

    add_custom_command ( TARGET ${Target_Name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${Target_Name}>" ${ANDROID_LIB_OUTPUT_DIRECTORY}
        )

    #
    # We strip the final .so file that will be put on the device, not the real one
    #

    add_custom_command ( TARGET ${Target_Name}
        POST_BUILD
        COMMAND ${ANDROID_STRIP} "${ANDROID_LIB_OUTPUT_DIRECTORY}/lib${Target_Name}.so"
        )

    #
    # Enable GDB remote debugging
    #
    android_ndk_gdb_enable(
        ${TARGET} "${BUILD_OUTDIR}"
        "${BUILD_OUTDIR}/libs"
        "${SOURCES}" "${LIBRARIES}"
        )
    android_ndk_gdb_debuggable("${TARGET}")


    # Install dependency libraries
    set ( ANDROID_DEPENDENCIES_STRING )
    foreach(lib ${Dependency_Libs})
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${lib} ${ANDROID_LIB_OUTPUT_DIRECTORY}
            )
	# Problem: generator strings
        # Solution: regex
        string ( REGEX REPLACE "\\$<TARGET_FILE:([A-Za-z0-9_\-]+)>" "\\1" _LIBNAME_STRIPPED "${lib}" )
	# Strip off lib* part, to avoid liblib* on Android side
	string ( REGEX REPLACE "lib([A-Za-z0-9_\-]+)" "\\1" _LIBNAME_STRIPPED "${_LIBNAME_STRIPPED}" )
        # Some dependencies are file paths, too!
        get_filename_component ( _LIBNAME_STRIPPED ${_LIBNAME_STRIPPED} NAME_WE )
        string ( CONCAT
            ANDROID_DEPENDENCIES_STRING
            "${ANDROID_DEPENDENCIES_STRING}"
            "\"${_LIBNAME_STRIPPED}\", "
             )
    endforeach()

    if(ANDROID_USE_SDL2_LAUNCH)
        # Insert details into files
        configure_file (
            "${ANDROID_PROJECT_CONFIG_DIR}/sdl2/AndroidManifest.xml.in"
            "${BUILD_OUTDIR}/AndroidManifest.xml"
            @ONLY
            )
        configure_file (
            "${ANDROID_PROJECT_CONFIG_DIR}/sdl2/SDLActivity.java.in"
            "${BUILD_OUTDIR}/src/org/libsdl/app/SDLActivity.java"
            @ONLY
            )
        configure_file (
            "${ANDROID_PROJECT_CONFIG_DIR}/sdl2/${ANDROID_STARTUP_ACTIVITY}.java.in"
            "${BUILD_OUTDIR}/src/${ANDROID_MAIN_PATH}/${ANDROID_STARTUP_ACTIVITY}.java"
            @ONLY
            )
    else()
        configure_file (
            "${ANDROID_PROJECT_CONFIG_DIR}/native/AndroidManifest.xml.in"
            "${BUILD_OUTDIR}/AndroidManifest.xml"
            @ONLY
            )
    endif()

    configure_file (
        "${ANDROID_PROJECT_CONFIG_DIR}/build.xml.in"
        "${BUILD_OUTDIR}/build.xml"
        @ONLY
        )
    configure_file (
        "${ANDROID_PROJECT_CONFIG_DIR}/project.properties.in"
        "${BUILD_OUTDIR}/project.properties"
        @ONLY
        )
    configure_file (
        "${ANDROID_PROJECT_CONFIG_DIR}/strings.xml.in"
        "${BUILD_OUTDIR}/res/values/strings.xml"
        @ONLY
        )

    file ( WRITE "${BUILD_OUTDIR}/local.properties"
        "sdk.dir=${ANDROID_SDK}")

    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ANDROID_ASSET_OUTPUT_DIRECTORY}" )

    # Install asset files
    foreach (resc ${ARGN})
        add_custom_command ( TARGET ${Target_Name}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${resc} .
            WORKING_DIRECTORY ${ANDROID_ASSET_OUTPUT_DIRECTORY}
            )
    endforeach()

    # Update Android project files
    add_custom_command ( TARGET ${Target_Name}
        COMMAND ${ANDROID_SDK_PROGRAM} update project --path ${BUILD_OUTDIR}
        PRE_BUILD
        WORKING_DIRECTORY ${BUILD_OUTDIR}
        )

    add_custom_command ( TARGET ${Target_Name}
	PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ANDROID_APK_OUTPUT_DIR}"
        )

    add_custom_command ( TARGET ${Target_Name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${Target_Name}>
            ${NDK_GDB_SOLIB_PATH}
        )


    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # In release-mode, we sign and align the APK manually
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND ${ANDROID_ANT_PROGRAM} release
            WORKING_DIRECTORY ${BUILD_OUTDIR}
            )
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND jarsigner
                -verbose
                -keystore ${ANDROID_APK_SIGN_KEY}
                -storepass $ENV{ANDROID_APK_SIGN_PASS}
                bin/${ANDROID_APK_NAME}
                ${ANDROID_APK_SIGN_ALIAS}

            WORKING_DIRECTORY ${BUILD_OUTDIR}
            )
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND ${ANDROID_ZIPALIGN} -v -f 4 bin/${ANDROID_APK_NAME} ${ANDROID_APK_FILE_OUTPUT}
            WORKING_DIRECTORY ${BUILD_OUTDIR}
            )
    else()
        # Debug mode is quite simple
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND ${ANDROID_ANT_PROGRAM} ${ANDROID_ANT_COMMON_PROPERTIES} debug
            WORKING_DIRECTORY ${BUILD_OUTDIR}
	        )
    endif()

    if(ANDROID_DEPLOY_APK)
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND
            ${ANDROID_ADB_PROGRAM} -s "${DEVICE_TARGET}" install -r "${ANDROID_APK_FILE_OUTPUT}"
            )
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND
            ${ANDROID_ADB_PROGRAM} shell am start
            -a "${ANDROID_AM_START_INTENT}"
            -n "${ANDROID_AM_START_ACTIVITY}"
            )
        android_ndk_gdb_debug(${Target_Name} "${BUILD_OUTDIR}" "${DEVICE_TARGET}")
    endif()

    install (
        FILES
        ${ANDROID_APK_FILE_OUTPUT}

        DESTINATION
        ${CMAKE_PACKAGED_OUTPUT_PREFIX}/android-apk
        )

endmacro()

macro(ANDROIDAPK_PACKAGE
        TARGET
        DOM_NAME TITLE COFFEE_VERSION_CODE COPYRIGHT COMPANY
        SOURCES
        BUNDLE_RSRCS
        BUNDLE_LIBS
        ICON_ASSET )


    if(ANDROID_USE_SDL2_LAUNCH)
        add_library(${TARGET} SHARED ${SOURCES} "${SDL2_ANDROID_MAIN_FILE}" )
    else()
        add_library(${TARGET} SHARED ${SOURCES}
            ${CMAKE_SOURCE_DIR}/coffee/core/private/plat/graphics/eglinit.cpp )

        target_link_libraries ( ${TARGET}
            AndroidCore
            EGL
            )
    endif()

    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

    # Lowercase the target name ofr package name
    string ( TOLOWER "${TARGET}" PACKAGE_SUFFIX )

    set ( DEPENDENCIES )

    list ( APPEND DEPENDENCIES $<TARGET_FILE:${TARGET}> )

    list ( APPEND DEPENDENCIES "${BUNDLE_LIBS}" )

    APK_PACKAGE_EXT(
        "${TARGET}"
        "${TITLE}"
        "${DOM_NAME}.${PACKAGE_SUFFIX}"
        "${COFFEE_VERSION_CODE}" "${COFFEE_BUILD_STRING}"
        "${ANDROID_NATIVE_API_LEVEL}" "${ANDROID_ABI}"
        "${DEPENDENCIES}"
        "${ICON_ASSET}"
        ${BUNDLE_RSRCS}
        )
endmacro()
