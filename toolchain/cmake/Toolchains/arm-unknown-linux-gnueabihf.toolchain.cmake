set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR armv7l )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake )

set ( CMAKE_C_COMPILER   "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++" )

set ( SHARED_FLAGS
    "-mtune=cortex-a8 -march=armv7-a+simd+vfpv3 -mfloat-abi=hard" 
    )

set ( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${SHARED_FLAGS}" CACHE STRING "" )
set ( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${SHARED_FLAGS}" CACHE STRING "" )

set ( SHARED_LINK_FLAGS
    "-static-libstdc++ ${CMAKE_SYSROOT}/usr/lib/libc.a"
    )

include ( ${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake )

#add_definitions ( -D__BEAGLEBONE__ -D__BEAGLEBONEBLACK__ )
