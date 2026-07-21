# Used for coffeecutie's own host-tools builds (ShaderCooker,
# TextureCompressor, etc. -- see .cmake/CMakePresets.linux.json's
# host-x64-linux-native preset). Deliberately just vcpkg's own stock
# x64-linux triplet shape, no VCPKG_CHAINLOAD_TOOLCHAIN_FILE, no
# VCPKG_C_FLAGS/VCPKG_CXX_FLAGS -- host tools run once per asset at build
# time, not perf-critical, so there's no reason to target any specific ISA
# level here.
#
# Existing on its own, under its own name, is the point: vcpkg's binary
# cache keys each package by (port, TRIPLET NAME, features, abi) -- a
# package built under plain "x64-linux" (vcpkg's own unmodified stock
# triplet, which every OTHER consumer of that triplet across the whole
# vcpkg ecosystem also shares) is a completely different cache entry from
# one built under "x64-linux-native", even with byte-identical triplet
# file content. Host tools were never actually pinned to a project-owned
# triplet before (host-x64-linux-native's preset chain never set
# VCPKG_TARGET_TRIPLET at all, silently falling back to plain "x64-linux")
# -- so shaderc/spirv-tools/abseil/protobuf etc. have been resolving to
# whatever got cached under "x64-linux" globally, built on some arbitrary
# machine at some arbitrary point in the past, and reused forever
# regardless of any -march= change on our side (confirmed: a local
# rebuild after removing -march=native from the CMake preset still
# resolved these packages via "Restored N package(s) from
# .../vcpkg/archives", i.e. cache-hit, never recompiled). Pointing this
# triplet's name at OUR OWN builds forces a fresh compile under whatever
# the current compiler's own safe default is, this time captured in a
# cache entry host tools actually own.
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)
