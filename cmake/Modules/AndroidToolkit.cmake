set ( ANDROID_ANT_PROGRAM NOTFOUND CACHE FILEPATH "ANT build program" )
set ( ANDROID_SDK_PROGRAM NOTFOUND CACHE FILEPATH "Android SDK android" )
set ( ANDROID_NDK_BUILD_PROGRAM NOTFOUND CACHE FILEPATH "Android NDK ndk-build" )
set ( ANDROID_ZIPALIGN NOTFOUND CACHE FILEPATH "Android zipalign" )

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
    $ENV{ANDROID_SDK}
    $ENV{ANDROID_HOME}

    PATH_SUFFIXES
    tools
    )

find_program ( ANDROID_NDK_BUILD_PROGRAM
    NAMES
    ndk-build

    PATHS
    $ENV{ANDROID_NDK}
    )

if ( NOT ANDROID_SDK_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android SDK, please specify ANDROID_SDK or ANDROID_HOME" )
endif()
if ( NOT ANDROID_NDK_BUILD_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android NDK, please specify ANDROID_NDK" )
endif()
if( NOT ANDROID_ANT_PROGRAM)
    message ( FATAL_ERROR "-- Failed to locate ANT program" )
endif()
