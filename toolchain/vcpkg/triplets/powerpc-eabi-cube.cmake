set(VCPKG_TARGET_ARCHITECTURE powerpc)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Baremetal)

set(VCPKG_DEP_INFO_OVERRIDE_VARS bare;gamecube;wii)
set(TOOLCHAIN_PREFIX powerpc-eabi)
set(VCPKG_OVERLAY_PORTS ${CMAKE_CURRENT_LIST_DIR}/../ports)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Toolchains/powerpc-eabi-cube.toolchain.cmake)

