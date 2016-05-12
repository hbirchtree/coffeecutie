set ( ANDROID_NDK_S NOTFOUND CACHE PATH "Android NDK path" )
set ( ANDROID_SDK_S NOTFOUND CACHE PATH "Android SDK path" )

if(DEFINED ENV{ANDROID_NDK})
    set ( ANDROID_NDK_S "$ENV{ANDROID_NDK}" )
endif()
if(DEFINED ENV{ANDROID_SDK})
    set ( ANDROID_SDK_S "$ENV{ANDROID_SDK}" )
endif()

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
    ${ANDROID_SDK_S}
    $ENV{ANDROID_HOME}

    PATH_SUFFIXES
    tools
    )

find_program ( ANDROID_NDK_BUILD_PROGRAM
    NAMES
    ndk-build

    PATHS
    ${ANDROID_NDK_S}
    )

file ( GLOB ANDROID_STRIP_CANDIDATES
    "${ANDROID_NDK_S}/toolchains/${ANDROID_TOOLCHAIN_NAME}/prebuilt/linux-x86_64/bin/*-strip")

set ( ANDROID_STRIP "${ANDROID_STRIP_CANDIDATES}" )

if ( NOT ANDROID_SDK_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android SDK, please specify ANDROID_SDK or ANDROID_HOME" )
endif()
if ( NOT ANDROID_NDK_BUILD_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android NDK, please specify ANDROID_NDK" )
endif()
if( NOT ANDROID_ANT_PROGRAM)
    message ( FATAL_ERROR "-- Failed to locate ANT program" )
endif()

message ( "Android strip program: ${ANDROID_STRIP}" )
