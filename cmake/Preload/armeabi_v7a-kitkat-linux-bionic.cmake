include ( android-common-pre )

# Android toolchain-specific
set ( ANDROID_NATIVE_API_LEVEL 19 CACHE STRING "" )
set ( ANDROID_ABI "armeabi-v7a with VFPV3" CACHE STRING "" )

string ( REGEX REPLACE "([a-zA-Z0-9_-]+)\ .*" "\\1" ANDROID_LIB_SUFFIX "${ANDROID_ABI}"  )
link_directories ( "${NATIVE_LIBRARY_DIR}/${ANDROID_LIB_SUFFIX}" )

include ( android-common-post )
