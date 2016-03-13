#!/bin/bash

source ./build-settings.rc

./openal-build.sh
./sdl-build.sh

mkdir -p ./libs/

mkdir -p ./build/libs/lib/
mkdir -p ./build/libs/include/

cp -r "$AL_INCLUDE_DIR" ./build/libs/include/
cp -r "$SDL2_INCLUDE_DIR" ./build/libs/include/

cp $HOME/libs/*.so ./build/libs/lib/
cp $HOME/libs/*.a ./build/libs/lib/

exit 0
