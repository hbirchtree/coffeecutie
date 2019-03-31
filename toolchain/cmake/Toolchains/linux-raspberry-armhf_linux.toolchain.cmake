set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR arm )
set ( CMAKE_SYSTEM_VERSION 1 )

set ( TOOLCHAIN_PREFIX "arm-linux-gnueabihf" )

set ( RASPBERRY TRUE )

set ( RASPBERRY_SDK "${RASPBERRY_SDK}" CACHE PATH "" )

set ( SYSROOT_PREFIX "/cross-rpi/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf" )
set ( CMAKE_SYSROOT "${SYSROOT_PREFIX}/${TOOLCHAIN_PREFIX}/sysroot" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++" )

set ( CMAKE_FIND_ROOT_PATH
    ${CMAKE_SYSROOT}
    ${RASPBERRY_SDK}
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/src/libs
    )

set ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

set ( CMAKE_LIBRARY_ARCHITECTURE "arm-linux-gnueabihf" CACHE STRING "" )

include_directories (
    ${RASPBERRY_SDK}/vc/include
    ${RASPBERRY_SDK}/vc/include/interface/vcos/pthreads
    ${RASPBERRY_SDK}/vc/include/interface/vmcs_host/linux
    ${RASPBERRY_SDK}/usr/include
    )

link_directories (
    ${RASPBERRY_SDK}/vc/lib
    ${RASPBERRY_SDK}/usr/lib
    )

add_definitions(-D__RASPBERRYPI__)
