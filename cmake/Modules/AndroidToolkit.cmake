find_package ( AndroidSDK )

message ("Android paths: NDK=${ANDROID_NDK}, SDK=${ANDROID_SDK}")

set ( ANDROID_ADB_PROGRAM
        "/usr/bin/adb"
        CACHE FILEPATH "ANT build program" )

set ( ANDROID_ANT_PROGRAM

        CACHE FILEPATH "ANT build program" )
set ( ANDROID_SDK_PROGRAM
        "${ANDROID_SDK}/tools/android${TOOL_OS_SUFFIX}"
        CACHE FILEPATH "Android SDK android" )
set ( ANDROID_NDK_BUILD_PROGRAM
        "${ANDROID_NDK}/ndk-build${TOOL_OS_SUFFIX}"
        CACHE FILEPATH "Android NDK ndk-build" )
set ( ANDROID_ZIPALIGN

        CACHE FILEPATH "Android zipalign" )
set ( ANDROID_STRIP
        "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-strip${TOOL_OS_SUFFIX}"
        CACHE FILEPATH "Android strip utility" )

find_program ( ANDROID_ANT_PROGRAM_TMP
    ant
    )

#find_program ( ANDROID_SDK_PROGRAM
#    NAMES
#    android
#
#    PATHS
#    $ENV{ANDROID_SDK}/tools
#    $ENV{ANDROID_HOME}/tools
#    )

#find_program ( ANDROID_NDK_BUILD_PROGRAM
#    NAMES
#    ndk-build${TOOLS_OS_SUFFIX}
#
#    PATHS
#    ${ANDROID_NDK_SEARCH_PATHS}
#    )

if ( ANDROID_ANT_PROGRAM_TMP )
    set ( ANDROID_ANT_PROGRAM "${ANDROID_ANT_PROGRAM_TMP}" )
endif()

mark_as_advanced (
    ANDROID_NDK_BUILD_PROGRAM
    ANDROID_ANT_PROGRAM
    ANDROID_ADB_PROGRAM
    ANDROID_SDK_PROGRAM
    ANDROID_STRIP
    ANDROID_ZIPALIGN
    )
