set(VCPKG_TARGET_ARCHITECTURE arm)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

set(VCPKG_DEP_INFO_OVERRIDE_VARS linux;arm;wayland)
set(TOOLCHAIN_PREFIX arm-buildroot-linux-gnueabihf)
set(VCPKG_OVERLAY_PORTS ${CMAKE_CURRENT_LIST_DIR}/../ports)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Toolchains/arm-unknown-linux-gnueabihf.toolchain.cmake)

