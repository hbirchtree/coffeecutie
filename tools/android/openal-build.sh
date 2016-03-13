#!/bin/bash

cd $HOME/lib/openal-soft/jni

# openal-soft is broken
ndk-build APP_ABI=arm64-v8a
cp libs/arm64-v8a/*.so $HOME/libs/

ndk-build APP_ABI=armeabi-v7a
cp libs/armeabi-v7a/*.so $HOME/libs/
