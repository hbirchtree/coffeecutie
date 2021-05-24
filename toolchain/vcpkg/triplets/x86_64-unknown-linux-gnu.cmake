set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

set(TOOLCHAIN_PREFIX x86_64-unknown-linux-gnu)
set(VCPKG_OVERLAY_PORTS ${CMAKE_CURRENT_LIST_DIR}/../ports)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Toolchains/x86_64-unknown-linux-gnu.toolchain.cmake)
