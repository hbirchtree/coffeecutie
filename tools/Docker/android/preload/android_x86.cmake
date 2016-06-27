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

# SDL2
set ( SDL2_LIBRARY
    "$ENV{LIBRARY_PREFIX}/x86/libSDL2.a"
    CACHE FILEPATH "" )
set ( SDL2_INCLUDE_DIR
    "$ENV{LIBRARY_PREFIX}/include"
    CACHE FILEPATH "" )

# OpenAL
set ( OPENAL_LIBRARY
    "$ENV{LIBRARY_PREFIX}/x86/libopenal.so"
    CACHE FILEPATH "" )
set ( OPENAL_INCLUDE_DIR
    "$ENV{LIBRARY_PREFIX}/include"
    CACHE FILEPATH "" )
