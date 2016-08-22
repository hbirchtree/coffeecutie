set ( CMAKE_SYSTEM_NAME Linux )
set ( TOOLCHAIN_PREFIX "arm-linux-gnueabihf" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++" )

set ( CMAKE_AR "${TOOLCHAIN_PREFIX}-ar" )
set ( CMAKE_STRIP "${TOOLCHAIN_PREFIX}-strip" )
set ( CMAKE_NM "${TOOLCHAIN_PREFIX}-nm" )

set ( CMAKE_FIND_ROO_PATH_MODE_PROGRAM NEVER )
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

include_directories ( BEFORE
    "/usr/lib/gcc-cross/${TOOLCHAIN_PREFIX}/5/include"
    "/usr/${TOOLCHAIN_PREFIX}/include/c++/5/${TOOLCHAIN_PREFIX}"
    )
link_directories (
    "/usr/lib/gcc-cross/${TOOLCHAIN_PREFIX}/5/"
    )
