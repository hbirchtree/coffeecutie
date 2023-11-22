set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CMAKE_SYSTEM_VERSION 1)

set(VCPKG_DEP_INFO_OVERRIDE_VARS linux;arm;arm64;armv8;aarch64;wayland)
set(TOOLCHAIN_PREFIX aarch64-buildroot-linux-gnu)
set(VCPKG_OVERLAY_PORTS ${CMAKE_CURRENT_LIST_DIR}/../ports)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Toolchains/aarch64-buildroot-linux-gnu.toolchain.cmake)

