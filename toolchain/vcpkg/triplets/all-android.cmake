#set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE $ENV{ANDROID_NDK}/build/cmake/android.toolchain.cmake CACHE FILEPATH "")
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Android)

#set(ANDROID_PLATFORM android-28 CACHE STRING "")
