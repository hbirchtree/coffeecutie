find_path ( ANDROID_SDK
    NAMES
    tools/android${TOOL_OS_SUFFIX}

    HINTS
    $ENV{ANDROID_SDK}
    $ENV{ANDROID_HOME}

    PATHS
    ~
    ~/.local
    ~/Library
    /opt
    /usr/local
    $ENV{ANDROID_SDK}
    $ENV{ANDROID_HOME}

    PATH_SUFFIXES
    android-sdk-linux
    android-sdk-windows
    android-sdk-macosx
    )

set ( ANDROID_SDK "${ANDROID_SDK}" CACHE PATH "Path to Android SDK" )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(AndroidSDK REQUIRED_VARS ANDROID_SDK)
