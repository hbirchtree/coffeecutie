#!/bin/bash
BUILD_DIR="$HOME/android-build"
ENV_FILE="$HOME/android-env.sh"

source "$ENV_FILE"

mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

cmake ../coffeecutie/ -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../coffeecutie/cmake/Toolchains/android.toolchain.cmake -DANDROID_STL=gnustl_static -DANDROID_NATIVE_API_LEVEL=19 -DOPENAL_LIBRARY=libs/openal-soft/libs/armeabi/libopenal.so -DOPENAL_INCLUDE_DIR=libs/openal-soft/jni/OpenAL/include/ -DSDL2_LIBRARY=../SDL2-2.0.4/build/org.libsdl.testgles/libs/armeabi-v7a/libSDL2.so -DSDL2_INCLUDE_DIR=../include-dir/ -DCOFFEE_BUILD_GLBINDING=OFF -DCOFFEE_BUILD_OPENVR=OFF -DCOFFEE_BUILD_QT=OFF -DCOFFEE_BUILD_ASSIMP=OFF -DCOFFEE_BUILD_FFMEDIA=OFF -DCOFFEE_BUILD_GLES=ON
