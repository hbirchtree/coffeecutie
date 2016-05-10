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


for arch_idx in $(seq 0 $((6-1)))
do
    # Create build directory
    BUILD_DIR="${BUILD_DIR_PREFIX}_${ARCHITECTURES[${arch_idx}]}"
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"

    API="${ANDROID_TARGET[${arch_idx}]}"
    ABI="${ANDROID_ABI[${arch_idx}]}"

    # Configure and build
    cmake "$PROJECT_DIR" -G "Ninja" \
        -DCMAKE_TOOLCHAIN_FILE="${ANDROID_TOOLCHAIN_FILE}" \
        -DANDROID_STL="${ANDROID_STL_IMPL}" \
        -DANDROID_NATIVE_API_LEVEL="${API}" \
        -DANDROID_ABI="${ABI}" \
        -DSDL2_LIBRARY="${LIBRARY_PREFIX}/${ABI}/${AL_LIBRARY_FILE_SO}" \
        -DOPENAL_INCLUDE_DIR="$AL_INCLUDE_DIR" \
        -DSDL2_LIBRARY="${LIBRARY_PREFIX}/${ABI}/${SDL2_LIBRARY_FILE_SO}" \
        -DSDL2_INCLUDE_DIR="$SDL2_INCLUDE_DIR" \
        -DCOFFEE_BUILD_GLES=ON \
        -DCOFFEE_BUILD_OPENSSL=OFF \
    && ninja

    [[ $? != 0 ]] && exit 1

done

echo "Output files:"

for arch in $(ARCHITECTURES[@])
do
    BUILD_DIR="${BUILD_DIR_PREFIX}_${ARCHITECTURES[${arch_idx}]}"
    echo "Architecture ${arch}:"
    echo ${BUILD_DIR}/lib/*.so
done

exit 0
