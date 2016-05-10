# How to operate the Docker container
 1. Build the container (found in tools/Docker/android )
 2. When finished, execute:

       ./install-libs.sh
       ./coffee-build.sh

This should give you a nice starting point for Android.

You could also just set up the environment natively.

In release mode, some environment variables are needed:
 - ANDROID_APK_SIGN_KEY : a keystore you want to use (by default ~/keystore/key.release)
 - ANDROID_APK_SIGN_ALIAS : alias for keystore
 - ANDROID_APK_SIGN_PASS : password for given keychain (not cached, no worries)

You might have to help the scripts to find the SDK, NDK and other utilities (zipalign etc.)
