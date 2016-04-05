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

set ( ANDROID_SDL_MAIN_UNIT "$ENV{HOME}/lib/SDL2/src/main/android/SDL_android_main.c" )
set ( ANDROID_ANT_PROGRAM "/usr/bin/ant" )
set ( ANDROID_SDK_PROGRAM "$ENV{ANDROID_SDK}/tools/android" )
set ( ANDROID_NDK_BUILD_PROGRAM "$ENV{ANDROID_NDK}/ndk-build" )
set ( ANDROID_ZIPALIGN $ENV{ZIPALIGN_UTIL} )

if ( NOT ANDROID_SDK_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android SDK, please specify ANDROID_SDK or ANDROID_HOME" )
endif()
if ( NOT ANDROID_NDK_BUILD_PROGRAM )
    message ( FATAL_ERROR "-- Failed to locate Android NDK, please specify ANDROID_NDK" )
endif()
if( NOT ANDROID_ANT_PROGRAM)
    message ( FATAL_ERROR "-- Failed to locate ANT program" )
endif()
