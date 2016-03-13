#!/bin/bash

BUILD_CONFIG="$HOME/build-settings.rc"

# Import build configuration
if [[ -e "$BUILD_CONFIG" ]]; then
	source "$BUILD_CONFIG"
else
	echo "Failed to locate build configuration ($BUILD_CONFIG)"
	exit 1
fi

if [[ -e "$PROJECT_DIR/CMakeLists.txt" ]]; then
	true
else
	echo "Failed to locate CMakeLists.txt"
	exit 1
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

BUILDERROR_OUT=builderrors.log
BUILDCONFIG_OUT=buildconfig.log

# Configure and build
cmake "$PROJECT_DIR" -G "Ninja" \
	-DCMAKE_TOOLCHAIN_FILE="$ANDROID_TOOLCHAIN_FILE" \
	-DANDROID_STL="$ANDROID_STL_IMPL" \
	-DANDROID_NATIVE_API_LEVEL="$ANDROID_TARGET" \
	-DOPENAL_LIBRARY="$AL_LIBRARY_FILE" \
	-DOPENAL_INCLUDE_DIR="$AL_INCLUDE_DIR" \
	-DSDL2_LIBRARY="$SDL2_LIBRARY_FILE" \
	-DSDL2_INCLUDE_DIR="$SDL2_INCLUDE_DIR" \
	-DCOFFEE_BUILD_GLES=ON \
	-DCOFFEE_BUILD_OPENSSL=OFF \
&& ninja
