#!/bin/bash

source ./build-settings.rc

basedir="./libs/"

subdirs[0]="lib/armeabi-v7a"
subdirs[1]="lib/arm64-v8a"
subdirs[2]="include"

for dr in ${subdirs[@]}
do
	mkdir -p "$basedir/$dr"
done

./openal-build.sh
./sdl-build.sh

include_directories[0]="$HOME/lib/openal-soft/jni/OpenAL/include/AL"
include_directories[1]="$HOME/lib/SDL2/include"

for inc in ${include_directories[@]}
do
	cp -r "$inc" $basedir/include/
done

mv $basedir/include/include $basedir/include/SDL2

mkdir -p ${BUILD_DIR}/libs/include

ln -s ${PROJECT_DIR}/libs/rapidjson/include/rapidjson ${BUILD_DIR}/libs/include

exit 0
