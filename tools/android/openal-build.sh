#!/bin/bash

cd $HOME/lib/openal-soft

# NOTE: OpenAL-soft is LGPLv3!
# This cannot be statically linked with Coffee
# But if you want to...

#OPENAL_PATCH="${HOME}/openal-soft-static-patch.patch"

#git apply --check "${OPENAL_PATCH}" \
#&& git apply "${OPENAL_PATCH}"

output_basedir="$HOME/libs/lib"

architectures[0]="armeabi-v7a"
architectures[1]="arm64-v8a"

for arch in ${architectures[@]}
do
	mkdir -p "$output_basedir/$arch/"
	
	ndk-build APP_ABI=$arch
        cp obj/local/$arch/*.a "$output_basedir/$arch/" 2>/dev/null
        cp libs/$arch/*.so "$output_basedir/$arch/" 2>/dev/null
done
