set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv6)

include(${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake)

set(CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++")

add_compile_options(
    # -marm
    -march=armv6z+fp
    # -mcpu=arm1176jzf-s
    # -mfloat-abi=hard
    # -mfpu=vfp
)
add_link_options(
    # -marm
    -march=armv6z+fp
    # -mcpu=arm1176jzf-s
    # -mfloat-abi=hard
    # -mfpu=vfp
    -latomic
)

include(${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake)
