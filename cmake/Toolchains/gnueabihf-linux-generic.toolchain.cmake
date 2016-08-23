set ( CMAKE_SYSTEM_NAME Linux )
set ( TOOLCHAIN_PREFIX "arm-linux-gnueabihf" )

set ( RASPBERRY_SDK "${RASPBERRY_SDK}" CACHE PATH "" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++" )

set ( CMAKE_AR "${TOOLCHAIN_PREFIX}-ar" )
set ( CMAKE_STRIP "${TOOLCHAIN_PREFIX}-strip" )
set ( CMAKE_NM "${TOOLCHAIN_PREFIX}-nm" )

set ( CMAKE_FIND_ROOT_PATH "${RASPBERRY_SDK}" )
set ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

include_directories ( BEFORE
    "${RASPBERRY_SDK}/usr/lib/gcc/${TOOLCHAIN_PREFIX}/4.9/include"
    "${RASPBERRY_SDK}/usr/include/${TOOLCHAIN_PREFIX}/c++/4.9"
    "${RASPBERRY_SDK}/usr/include"
    )
link_directories (
    "/usr/lib/gcc-cross/${TOOLCHAIN_PREFIX}/5/"
    )
