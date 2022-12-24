include(${CMAKE_CURRENT_LIST_DIR}/all-android.cmake)
set(VCPKG_TARGET_ARCHITECTURE arm-neon)
set(VCPKG_TARGET_TRIPLET arm-neon-android)
set(ANDROID_ABI armeabi-v7a CACHE STRING "")
