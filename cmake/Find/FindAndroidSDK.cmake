set ( ANDROID_SDK CACHE PATH "Path to Android SDK" )

find_path ( ANDROID_SDK
    NAMES
    tools/android${TOOL_OS_SUFFIX}

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

mark_as_advanced ( ANDROID_SDK )
