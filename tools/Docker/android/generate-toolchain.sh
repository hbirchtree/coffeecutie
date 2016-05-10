#!/bin/bash

GENERATOR="$ANDROID_NDK/build/tools/make-standalone-toolchain.sh"

function print_help()
{
	echo "Usage: $0 [API level] [PATH file]"
	echo "Example: $0 19 android-19.rc"
}

if [ -z "$1" ]
then
	echo "API level undefined"
	echo ""
	print_help
	exit 1
fi

# Default architecture is 32-bit ARMEABI
ARCH=arm

if [ -n "${2+1}" ]
then
	ARCH=$2
fi

OUTDIR=$HOME/toolchains

mkdir -p $HOME/toolchains 2>/dev/null

if [[ -e "$GENERATOR" ]]
then
	TC_DESTINATION="$OUTDIR/ndk-toolchain-$1-$ARCH/"
	"$GENERATOR" --platform=android-$1 --install-dir="$TC_DESTINATION" --arch=$ARCH
	
	if [ $? -ne 0 ]
	then
		exit 1
	fi
	
	echo "NDK_STANDALONE=$TC_DESTINATION" >> "$OUTDIR/linux-android-$1-$ARCH.rc"
	echo "PATH=\$NDK_STANDALONE/bin:\$PATH" >> "$OUTDIR/linux-android-$1-$ARCH.rc"
	
else
	exit 1
fi
