set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR armv7l )

set ( TOOLCHAIN_ARCH "arm-none-linux-gnueabihf" )
set ( TOOLCHAIN_PREFIX "${TOOLCHAIN_ARCH}-" )

set ( BEAGLE TRUE )

set ( LIBC_SYSROOT /usr/local/gcc-arm/${TOOLCHAIN_ARCH}/libc )

set ( CMAKE_C_COMPILER   "${TOOLCHAIN_PREFIX}gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++" )

set ( SHARED_FLAGS "-mtune=cortex-a8 -march=armv7-a+simd+vfpv3 -mfloat-abi=hard" )

set ( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${SHARED_FLAGS}" CACHE STRING "" )
set ( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${SHARED_FLAGS}" CACHE STRING "" )

set ( SHARED_LINK_FLAGS "-static-libgcc -static-libstdc++ /usr/local/gcc-arm/${TOOLCHAIN_ARCH}/libc/usr/lib/libc.a" )

set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${SHARED_FLAGS} ${SHARED_LINK_FLAGS}" CACHE STRING "" )
set ( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${SHARED_FLAGS} ${SHARED_LINK_FLAGS}" CACHE STRING "" )
set ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${SHARED_FLAGS} ${SHARED_LINK_FLAGS}" CACHE STRING "" )

if(NOT SGX_SYSROOT)
    # Default sysroot location in a Docker container
    # Contains EGL, GLESv2 etc.
    set ( SGX_SYSROOT /sysroots/sgx-root )
endif()
if(NOT DEB_SYSROOT)
    # Contains sys libs for Wayland and DRM
    set ( DEB_SYSROOT /sysroots/${TOOLCHAIN_ARCH} )
endif()

set ( CMAKE_FIND_ROOT_PATH
    ${CMAKE_SYSROOT}
    ${CMAKE_SYSROOT}/usr
    ${SGX_SYSROOT}
    ${DEB_SYSROOT}/usr
    ${DEB_SYSROOT}
    )

set ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
#set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
#set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

set ( CMAKE_LIBRARY_ARCHITECTURE "${TOOLCHAIN_ARCH}" CACHE STRING "" )

include_directories (
    ${LIBC_SYSROOT}/usr/include
    ${SGX_SYSROOT}/include
    ${DEB_SYSROOT}/usr/include
    )
link_directories (
    ${LIBC_SYSROOT}/lib
    ${LIBC_SYSROOT}/usr/lib
    ${SGX_SYSROOT}/lib
    )

set ( CMAKE_LIBRARY_PATH
    ${LIBC_SYSROOT}/lib
    ${LIBC_SYSROOT}/usr/lib
    ${SGX_SYSROOT}/lib
    ${DEB_SYSROOT}/usr/lib/${TOOLCHAIN_ARCH}
    )

add_definitions(-D__BEAGLEBONE__ -D__BEAGLEBONEBLACK__)
