set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE $ENV{ANDROID_NDK}/build/cmake/android.toolchain.cmake)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Android)

if(PORT STREQUAL "openal-soft")
  set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()
