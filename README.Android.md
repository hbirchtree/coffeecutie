# Port details
Based on SDL2 Android examples originally, uses it as a base for working at all. Display etc. is mostly handled by SDL2.
A newer, mostly-native solution will be added in the future in order to boost performance and decrease startup time. Will also be more feature-complete.

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
