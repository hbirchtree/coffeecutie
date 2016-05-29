find_package ( AndroidSDK )

message ("Android paths: NDK=${ANDROID_NDK}, SDK=${ANDROID_SDK}")

set ( ANDROID_ANT_PROGRAM NOTFOUND CACHE FILEPATH "ANT build program" )
set ( ANDROID_SDK_PROGRAM NOTFOUND CACHE FILEPATH "Android SDK android" )
set ( ANDROID_NDK_BUILD_PROGRAM NOTFOUND CACHE FILEPATH "Android NDK ndk-build" )
set ( ANDROID_ZIPALIGN NOTFOUND CACHE FILEPATH "Android zipalign" )
set ( ANDROID_STRIP NOTFOUND CACHE FILEPATH "Android strip utility" )

find_program ( ANDROID_ANT_PROGRAM
    NAMES
    ant

    PATHS
    /usr
    /usr/local

    PATH_SUFFIXES
    bin
    )

find_program ( ANDROID_SDK_PROGRAM
    NAMES
    android

    PATHS
    $ENV{ANDROID_SDK}/tools
    $ENV{ANDROID_HOME}/tools
    )

find_program ( ANDROID_NDK_BUILD_PROGRAM
    NAMES
    ndk-build${TOOLS_OS_SUFFIX}

    PATHS
    ${ANDROID_NDK_SEARCH_PATHS}
    )

set ( ANDROID_STRIP "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-strip${TOOL_OS_SUFFIX}" )

if ( NOT ANDROID_SDK_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android SDK, please specify ANDROID_SDK or ANDROID_HOME: ${ANDROID_SDK_PROGRAM}" )
endif()
if ( NOT ANDROID_NDK_BUILD_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android NDK, please specify ANDROID_NDK: ${ANDROID_NDK_BUILD_PROGRAM}" )
endif()
if( NOT ANDROID_ANT_PROGRAM)
    message ( FATAL_ERROR "-- Failed to locate ANT program" )
endif()

message ( "Android strip program: ${ANDROID_STRIP}" )

mark_as_advanced (
    ANDROID_NDK_BUILD_PROGRAM
    ANDROID_ANT_PROGRAM
    ANDROID_SDK_PROGRAM
    ANDROID_STRIP
    ANDROID_ZIPALIGN
    )
