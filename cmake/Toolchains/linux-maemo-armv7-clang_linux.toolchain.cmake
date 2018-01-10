set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR arm )
set ( TOOLCHAIN_PREFIX "arm-linux-gnueabi" )

#set ( CMAKE_SYSROOT "/home/havard/Code/Maemo5/maemo-root" )

set ( MAEMO TRUE CACHE BOOL "" )

set ( MAEMO_SDK "${MAEMO_SDK}" CACHE PATH "" )

set ( CMAKE_C_COMPILER "clang" )
set ( CMAKE_CXX_COMPILER "clang++" )

set ( CMAKE_C_COMPILER_TARGET ${TOOLCHAIN_PREFIX} )
set ( CMAKE_CXX_COMPILER_TARGET ${TOOLCHAIN_PREFIX} )

#set ( CMAKE_AR "/usr/bin/ar" CACHE FILEPATH "" )
set ( CMAKE_LINKER "${TOOLCHAIN_PREFIX}-ld" CACHE FILEPATH "" )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=cortex-a8" )

add_definitions(-D__MAEMOFREMANTLE__ -D__MAEMO__)

include_directories ( /usr/arm-linux-gnueabi/include/c++/5/arm-linux-gnueabi/ /usr/arm-linux-gnueabi/include )
