#!/bin/bash

source ./build-settings.rc

basedir="./libs/"

subdirs[0]="lib/armeabi-v7a"
subdirs[1]="lib/arm64-v8a"
subdirs[2]="include"

for dr in ${subdirs[@]}
do
	mkdir -p "$dr"
done

./openal-build.sh
./sdl-build.sh

include_directories[0]="$AL_INCLUDE_DIR"
include_directories[1]="$SDL2_INCLUDE_DIR"

for inc in ${include_directories[@]}
do
	cp -r "$inc" ./libs/include/
done

exit 0
