set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)

if(PORT STREQUAL "openal-soft")
  set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()
# Chainloaded toolchain skips vcpkg's -fPIC; static deps end up in shared ports
set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DCMAKE_POSITION_INDEPENDENT_CODE=ON)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

set(TOOLCHAIN_PREFIX x86_64-unknown-linux-gnu)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Toolchains/x86_64v2-buildroot-linux-gnu.toolchain.cmake)
