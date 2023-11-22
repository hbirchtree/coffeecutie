set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv8)

include(${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake)

set(CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++")

add_compile_options(
    -march=armv8-a
)
add_link_options(
    -march=armv8-a
  # -static-libstdc++ ${CMAKE_SYSROOT}/usr/lib/libc.a
)

include(${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake)
