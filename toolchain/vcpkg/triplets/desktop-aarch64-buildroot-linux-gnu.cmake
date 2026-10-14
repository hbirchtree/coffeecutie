set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

if(PORT STREQUAL "openal-soft")
  set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()
# Chainloaded toolchain skips vcpkg's -fPIC; static deps end up in shared ports
set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DCMAKE_POSITION_INDEPENDENT_CODE=ON)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CMAKE_SYSTEM_VERSION 1)

set(VCPKG_DEP_INFO_OVERRIDE_VARS linux;arm;arm64;armv8;aarch64;wayland;x11)
set(TOOLCHAIN_PREFIX aarch64-buildroot-linux-gnu)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Toolchains/aarch64-buildroot-linux-gnu.toolchain.cmake)

