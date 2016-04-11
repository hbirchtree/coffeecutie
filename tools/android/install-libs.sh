#!/bin/bash

source ./build-settings.rc

basedir="./libs"

subdirs[0]="lib/armeabi-v7a"
subdirs[1]="lib/arm64-v8a"
subdirs[2]="lib/x86"
subdirs[3]="lib/x86_64"
subdirs[4]="include"

for dr in ${subdirs[@]}
do
        mkdir -p "${basedir}/$dr"
done

./openal-build.sh
./sdl-build.sh

include_directories[0]="${HOME}/lib/openal-soft/jni/OpenAL/include/AL"
include_directories[1]="${HOME}/lib/SDL2/include"

include_targets[0]="AL"
include_targets[1]="SDL2"

num_includes=2

for i in $(seq 0 $(($num_includes-1)) )
do
	inc=${include_directories[$i]}
	inc_target=${include_targets[$i]}
        ln -s "${inc}" ${basedir}/include/${inc_target}
done

for arch in ${ANDROID_LIB_ARCH[@]}
do
    mkdir -p "${BUILD_DIR_PREFIX}_${arch}/libs"
    ln -sf ${PROJECT_DIR}/libs/rapidjson/include/rapidjson ${BUILD_DIR_PREFIX}_${arch}/libs/include
done

exit 0
