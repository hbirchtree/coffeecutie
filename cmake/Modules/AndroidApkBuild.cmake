include ( AndroidToolkit )

# APK signing

set ( ANDROID_APK_SIGN_KEY "~/keystore/key.release" CACHE FILEPATH "Android signing key" )

set ( ANDROID_APK_SIGN_ALIAS "$ENV{ANDROID_SIGN_ALIAS}" CACHE STRING "Android signing key alias" )

# Misc properties

set ( ANDROID_TEMPLATE_PROJECT "${CMAKE_SOURCE_DIR}/cmake/Platform/Android" )

set ( APK_OUTPUT_DIR "${CMAKE_BINARY_DIR}/packaged/apk" )

# For valid options, see:
# http://developer.android.com/guide/topics/manifest/activity-element.html
set ( ANDROID_ORIENTATION_MODE "sensorLandscape" )

set ( ANDROID_ACTIVITY_NAME )

set ( ANDROID_API_MIN_TARGET )
set ( ANDROID_API_TARGET )

# Name of package in package manager
set ( ANDROID_PACKAGE_NAME )
# Name of application in menus
set ( ANDROID_APPLICATION_NAME "Coffee App" )

# Version of APK file, should be consistent
set ( ANDROID_VERSION_CODE "1" )
set ( ANDROID_VERSION_NAME "1.0" )

set ( ANDROID_PROJECT_INPUT ${PROJECT_SOURCE_DIR}/cmake/Platform/Android/ )

set ( ANDROID_PROJECT_TEMPLATE_DIR ${ANDROID_PROJECT_INPUT}/Template )

set ( ANDROID_BUILD_OUTPUT ${PROJECT_BINARY_DIR}/deploy/android/ )

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
macro(PACKAGE_APK Target_Name App_Name Pkg_Name Version_Int Version_Str Api_Target Api_Arch Dependency_Libs )
    message ( "APK: Generating ${Pkg_Name} (${Api_Arch})" )

    set ( ANDROID_PACKAGE_NAME ${Pkg_Name} )

    set ( ANDROID_STARTUP_ACTIVITY "CoffeeActivity" )

    set ( ANDROID_ACTIVITY_NAME "${Pkg_Name}.${ANDROID_STARTUP_ACTIVITY}" )

    set ( ANDROID_APK_NAME "${Pkg_Name}.${ANDROID_STARTUP_ACTIVITY}-release-unsigned.apk" )

    set ( ANDROID_APPLICATION_NAME ${App_Name} )

    set ( ANDROID_VERSION_CODE ${Version_Int} )
    set ( ANDROID_VERSION_NAME ${Version_Str} )

    set ( ANDROID_API_TARGET ${Api_Target} )
    set ( ANDROID_API_MIN_TARGET "16" )

    set ( RELEASE_PREFIX )

    if( CMAKE_BUILD_TYPE STREQUAL "Release" )
        set ( RELEASE_PREFIX "rel" )
    else()
        set ( RELEASE_PREFIX "dbg" )
    endif()

    set ( ANDROID_APK_FILE_OUTPUT "${APK_OUTPUT_DIR}/${ANDROID_PACKAGE_NAME}_${RELEASE_PREFIX}.apk" )

    set( BUILD_OUTDIR ${ANDROID_BUILD_OUTPUT}/${Target_Name} )

    set ( ANDROID_LIB_OUTPUT_DIRECTORY ${BUILD_OUTDIR}/libs/${Api_Arch} )
    set ( ANDROID_ASSET_OUTPUT_DIRECTORY ${BUILD_OUTDIR}/assets )

    # Where the primary class is found, also decides names of package
    set ( ANDROID_MAIN_PATH )

    string ( REGEX REPLACE "\\." "/" ANDROID_MAIN_PATH "${ANDROID_PACKAGE_NAME}" )

    # Create build directory
    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_OUTDIR}
        )
    # Install base template files
    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ANDROID_PROJECT_INPUT}/Template ${BUILD_OUTDIR}
        )

    # Create library directory
    add_custom_command ( TARGET ${Target_Name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_LIB_OUTPUT_DIRECTORY}
        )

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

    # Insert details into files
    configure_file (
        "${ANDROID_PROJECT_INPUT}/Config/AndroidManifest.xml.in"
        "${BUILD_OUTDIR}/AndroidManifest.xml"
        @ONLY
        )
    configure_file (
        "${ANDROID_PROJECT_INPUT}/Config/build.xml.in"
        "${BUILD_OUTDIR}/build.xml"
        @ONLY
        )
    configure_file (
        "${ANDROID_PROJECT_INPUT}/Config/project.properties.in"
        "${BUILD_OUTDIR}/project.properties"
        @ONLY
        )
    configure_file (
        "${ANDROID_PROJECT_INPUT}/Config/strings.xml.in"
        "${BUILD_OUTDIR}/res/values/strings.xml"
        @ONLY
        )
    configure_file (
        "${ANDROID_PROJECT_INPUT}/Config/SDLActivity.java.in"
        "${BUILD_OUTDIR}/src/org/libsdl/app/SDLActivity.java"
        @ONLY
        )
    configure_file (
        "${ANDROID_PROJECT_INPUT}/Config/${ANDROID_STARTUP_ACTIVITY}.java.in"
        "${BUILD_OUTDIR}/src/${ANDROID_MAIN_PATH}/${ANDROID_STARTUP_ACTIVITY}.java"
        @ONLY
        )

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
	COMMAND ${CMAKE_COMMAND} -E make_directory "${APK_OUTPUT_DIR}"
        )

    set ( ANDROID_ANT_COMMON_PROPERTIES -Dout.final.file="${ANDROID_APK_FILE_OUTPUT}" )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND ${ANDROID_ANT_PROGRAM} release
            WORKING_DIRECTORY ${BUILD_OUTDIR}
            )
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND jarsigner -verbose -keystore ${ANDROID_APK_SIGN_KEY} -storepass $ENV{ANDROID_APK_SIGN_PASS} bin/${ANDROID_APK_NAME} ${ANDROID_APK_SIGN_ALIAS}
            WORKING_DIRECTORY ${BUILD_OUTDIR}
            )
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND ${ANDROID_ZIPALIGN} -v -f 4 bin/${ANDROID_APK_NAME} ${ANDROID_APK_FILE_OUTPUT}
            WORKING_DIRECTORY ${BUILD_OUTDIR}
            )
    else()
        add_custom_command ( TARGET ${Target_Name}
            POST_BUILD
            COMMAND ${ANDROID_ANT_PROGRAM} ${ANDROID_ANT_COMMON_PROPERTIES} debug
            WORKING_DIRECTORY ${BUILD_OUTDIR}
	    )
    endif()

endmacro()
