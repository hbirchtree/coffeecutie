set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7l)

include(${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake)

set(CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++")

# -Wno-psabi silences GCC 7.1 "parameter passing ... changed" notes; the ABI
# change is internal and consistent across this toolchain, so it's harmless here.
add_compile_options(-mtune=cortex-a8 -march=armv7-a+simd+vfpv3 -mfloat-abi=hard -Wno-psabi)
add_link_options(
  -mtune=cortex-a8 -march=armv7-a+simd+vfpv3 -mfloat-abi=hard
  # -static-libstdc++ ${CMAKE_SYSROOT}/usr/lib/libc.a
)
include(${CMAKE_CURRENT_LIST_DIR}/common/gold-options.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake)
