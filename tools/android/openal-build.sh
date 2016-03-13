#!/bin/bash

cd $HOME/lib/openal-soft

output_basedir="$HOME/libs/lib"

architectures[0]="armeabi-v7a"
architectures[1]="arm64-v8a"

for arch in ${architectures[@]}
do
	mkdir -p "$output_basedir/$arch/"
	
	ndk-build APP_ABI=$arch
	cp libs/$arch/* "$output_basedir/$arch/"
done
