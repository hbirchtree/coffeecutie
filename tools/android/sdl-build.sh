#!/bin/bash

cd $HOME/lib/SDL2
export NDK_STANDALONE="$HOME/"

ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=android-19 APP_ABI=armeabi-v7a
cp libs/armeabi-v7a/*.so $HOME/libs/
cp libs/armeabi-v7a/*.a $HOME/libs/
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=android-21 APP_ABI=arm64-v8a
cp libs/arm64-v8a/*.so $HOME/libs/
cp libs/arm64-v8a/*.a $HOME/libs/
