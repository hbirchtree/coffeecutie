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

# Typical errors (or mistakes, whatever)
## Not giving SDL_android_main.c to SDL2_ANDROID_MAIN_FILE

    07-25 03:06:38.894 28327 28344 E AndroidRuntime: FATAL EXCEPTION: SDLThread
    07-25 03:06:38.894 28327 28344 E AndroidRuntime: Process: org.coffee.gleambasetest_rhi, PID: 28327
    07-25 03:06:38.894 28327 28344 E AndroidRuntime: Theme: themes:{default=, fontPkg:system}
    07-25 03:06:38.894 28327 28344 E AndroidRuntime: java.lang.UnsatisfiedLinkError: No implementation found for int org.libsdl.app.SDLActivity.nativeInit(java.lang.Object) (tried Java_org_libsdl_app_SDLActivity_nativeInit and Java_org_libsdl_app_SDLActivity_nativeInit__Ljava_lang_Object_2)
    07-25 03:06:38.894 28327 28344 E AndroidRuntime: 	at org.libsdl.app.SDLActivity.nativeInit(Native Method)
    07-25 03:06:38.894 28327 28344 E AndroidRuntime: 	at org.libsdl.app.SDLMain.run(SDLActivity.java:953)
    07-25 03:06:38.894 28327 28344 E AndroidRuntime: 	at java.lang.Thread.run(Thread.java:818)
    07-25 03:06:38.899  5387  7747 W ActivityManager:   Force finishing activity org.coffee.gleambasetest_rhi/.CoffeeActivity
