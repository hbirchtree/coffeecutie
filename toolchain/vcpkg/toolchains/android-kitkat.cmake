# vcpkg chainload for Android KitKat (API 19) built with a split toolchain:
# the newest NDK's clang against NDK 25's API 19 sysroot, plus a modern libc++
# built from source against that sysroot.
#
# NDK r26 and later dropped API < 21, so r25's clang 14 is normally the ceiling
# for KitKat. Only the sysroot has to come from r25 though, and libc++ is ours,
# so the compiler is free to move forward.
#
# The pieces that depend on the built libc++ live in a generated overlay, so
# this file and the standalone toolchain emitted next to it stay in sync.
# Generate them with:
#
#     toolchain/android/build-kitkat-libcxx.sh

# android.cmake picks the STL from VCPKG_CRT_LINKAGE and sets it as a
# non-FORCE cache entry. Claiming the cache entry first wins, and matters:
# creating a cache entry also drops any plain variable of the same name.
set(ANDROID_STL none CACHE STRING "" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/android.cmake")

get_filename_component(_kk_overlay
    "${CMAKE_CURRENT_LIST_DIR}/../../../multi_build/compilers/android/kitkat-clang21/kitkat-libcxx.cmake"
    ABSOLUTE)

if(NOT EXISTS "${_kk_overlay}")
    message(FATAL_ERROR
        "KitKat libc++ has not been built yet.\n"
        "Expected: ${_kk_overlay}\n"
        "Run: toolchain/android/build-kitkat-libcxx.sh")
endif()

include("${_kk_overlay}")
