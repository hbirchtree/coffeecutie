#!/bin/bash

cd $HOME/lib/SDL2
export NDK_STANDALONE="$HOME/"

architectures[0]="armeabi-v7a"
architectures[1]="arm64-v8a"

versions[0]="android-19"
versions[1]="android-21"

num_architectures=2

for arch_idx in $(seq 0 $(($num_architectures - 1)))
do
	arch=${architectures[$arch_idx]}
	ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=${versions[$arch_idx]} APP_ABI=$arch
	cp libs/$arch/*.so $HOME/libs/lib/$arch/
	cp libs/$arch/*.a $HOME/libs/lib/$arch/
done


