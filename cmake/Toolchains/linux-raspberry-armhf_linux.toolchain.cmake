set ( CMAKE_SYSTEM_NAME Linux )
set ( TOOLCHAIN_PREFIX "arm-linux-gnueabihf" )

set ( RASPBERRY TRUE )

set ( RASPBERRY_SDK "${RASPBERRY_SDK}" CACHE PATH "" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++" )

set ( CMAKE_FIND_ROOT_PATH "${RASPBERRY_SDK};${NATIVE_LIBRARY_DIR};${COFFEE_ROOT_DIR};${CMAKE_SOURCE_DIR}/libs" )
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
link_directories ( ${RASPBERRY_SDK}/vc/lib ${RASPBERRY_SDK}/usr/lib )

add_definitions(-D__RASPBERRYPI__)
