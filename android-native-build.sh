#!/bin/bash
PROJECT_DIR="$HOME/coffeecutie"
BUILD_DIR="$HOME/android-build"
ENV_FILE="$HOME/android-env.sh"

#Stock SDL2 built for Android. Should have built it manually, but this is temporary.
SDL2_INCLUDE_DIR="$HOME/include-dir"
SDL2_LIBRARY_FILE="$HOME/SDL2-2.0.4/build/org.libsdl.testgles/libs/armeabi-v7a/libSDL2.so"

#OpenAL, typically sourced from https://github.com/AerialX/openal-soft-android
AL_LIBRARY_FILE="$BUILD_DIR/libs/openal-soft/libs/armeabi/libopenal.so"
AL_INCLUDE_DIR="$BUILD_DIR/libs/openal-soft/jni/OpenAL/include/"

#We are using a custom toolchain made for Android, sourced from https://github.com/taka-no-me/android-cmake
ANDROID_TOOLCHAIN_FILE="$PROJECT_DIR/cmake/Toolchains/android.toolchain.cmake"
#This is the only reasonable STL implementation that works with C++11 features. The others are dead to me.
ANDROID_STL_IMPL="gnustl_static"
#API level 19 works with all Android version past and current. Levels 20+ don't because of kernel/ABI changes
ANDROID_TARGET="19"

#Import Android build environment
source "$ENV_FILE"

#Update the Android SDK, most likely nothing new
android update sdk --filter "tools,build-tools-23.0.2,platform-tools,android-19" -u

#Make build directory if non-existent, don't care about error
mkdir "$BUILD_DIR" 2>/dev/null

#We build our dear project in the specified build directory
cd "$BUILD_DIR"
cmake ../coffeecutie/ -G "Ninja" \
\ #Android has a vastly different toolchain. We also target KitKat for various reasons.
	-DCMAKE_TOOLCHAIN_FILE="$ANDROID_TOOLCHAIN_FILE" \
	-DANDROID_STL="$ANDROID_STL_IMPL" \
	-DANDROID_NATIVE_API_LEVEL="$ANDROID_TARGET" \
\ #OpenAL, the Android version with OpenSL integration
	-DOPENAL_LIBRARY="$AL_LIBRARY_FILE" \
	-DOPENAL_INCLUDE_DIR="$AL_INCLUDE_DIR" \
\ #SDL2 used for a lot of Androidy-things
	-DSDL2_LIBRARY="$SDL2_LIBRARY_FILE" \
	-DSDL2_INCLUDE_DIR="$SDL2_INCLUDE_DIR" \
\ #All the nice things we can't have
	-DCOFFEE_BUILD_GLBINDING=OFF \
	-DCOFFEE_BUILD_OPENVR=OFF \
	-DCOFFEE_BUILD_QT=OFF \
	-DCOFFEE_BUILD_ASSIMP=OFF \
	-DCOFFEE_BUILD_FFMEDIA=OFF \
\ #What we do have
	-DCOFFEE_BUILD_GLES=ON
ninja
