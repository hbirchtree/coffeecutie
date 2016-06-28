set ( CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/cmake/Toolchains/android.toolchain.cmake"
    CACHE FILEPATH "" )

set ( CMAKE_INSTALL_PREFIX
    "${CMAKE_BINARY_DIR}/out"
    CACHE PATH "")

# Android toolchain-specific
set ( ANDROID_STL "gnustl_static" CACHE STRING "" )
set ( ANDROID_NATIVE_API_LEVEL "19" CACHE STRING "" )
set ( ANDROID_ABI "x86" CACHE STRING "" )
