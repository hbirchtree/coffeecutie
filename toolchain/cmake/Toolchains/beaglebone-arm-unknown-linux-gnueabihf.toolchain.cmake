set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR armv7l )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake )

set ( CMAKE_C_COMPILER   "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++" )

add_definitions (
    -D__BEAGLEBONE__=1
    -D__BEAGLEBONEBLACK__=1
    )

add_compile_options (
    -mtune=cortex-a8
    -march=armv7-a+simd+vfpv3
    -mfloat-abi=hard
    )
add_link_options (
    -mtune=cortex-a8
    -march=armv7-a+simd+vfpv3
    -mfloat-abi=hard
#    -static-libstdc++
#    ${CMAKE_SYSROOT}/usr/lib/libc.a
    )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake )

