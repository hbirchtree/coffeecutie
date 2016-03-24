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
mkdir -p "$BUILD_DIR_32"
cd "$BUILD_DIR_32"

# Configure and build
cmake "$PROJECT_DIR" -G "Ninja" \
	-DCMAKE_TOOLCHAIN_FILE="$ANDROID_TOOLCHAIN_FILE" \
	-DANDROID_STL="$ANDROID_STL_IMPL" \
	-DANDROID_NATIVE_API_LEVEL="$ANDROID_TARGET_32" \
	-DANDROID_ABI="$ANDROID_ABI_32" \
	-DOPENAL_LIBRARY="$AL_LIBRARY_FILE_32" \
	-DOPENAL_INCLUDE_DIR="$AL_INCLUDE_DIR" \
	-DSDL2_LIBRARY="$SDL2_LIBRARY_FILE_32" \
	-DSDL2_INCLUDE_DIR="$SDL2_INCLUDE_DIR" \
	-DCOFFEE_BUILD_GLES=ON \
	-DCOFFEE_BUILD_OPENSSL=OFF \
&& ninja

[[ $? != 0 ]] && exit 1

# Create build directory
mkdir -p "$BUILD_DIR_64"
cd "$BUILD_DIR_64"

# Configure and build
cmake "$PROJECT_DIR" -G "Ninja" \
	-DCMAKE_TOOLCHAIN_FILE="$ANDROID_TOOLCHAIN_FILE" \
	-DANDROID_STL="$ANDROID_STL_IMPL" \
	-DANDROID_NATIVE_API_LEVEL="$ANDROID_TARGET_64" \
	-DANDROID_ABI="$ANDROID_ABI_64" \
	-DOPENAL_LIBRARY="$AL_LIBRARY_FILE_64" \
	-DOPENAL_INCLUDE_DIR="$AL_INCLUDE_DIR" \
	-DSDL2_LIBRARY="$SDL2_LIBRARY_FILE_64" \
	-DSDL2_INCLUDE_DIR="$SDL2_INCLUDE_DIR" \
	-DCOFFEE_BUILD_GLES=ON \
	-DCOFFEE_BUILD_OPENSSL=OFF \
&& ninja

[[ $? != 0 ]] && exit 1

echo "Output files:"

echo "32-bit:"
echo $BUILD_DIR_32/lib/*.so

echo "64-bit:"
echo $BUILD_DIR_64/lib/*.so

exit 0
