set ( CMAKE_SYSTEM_NAME Linux )
set ( TOOLCHAIN_PREFIX "arm-unknown-linux-gnueabi" )

set ( MAEMO TRUE CACHE BOOL "" )

set ( MAEMO_SDK "${MAEMO_SDK}" CACHE PATH "" )

set ( CMAKE_C_COMPILER "/usr/bin/${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "/usr/bin/${TOOLCHAIN_PREFIX}-g++" )
set ( CMAKE_AR "/usr/bin/ar" CACHE FILEPATH "" )
set ( CMAKE_LINKER "/usr/bin/${TOOLCHAIN_PREFIX}-gcc" CACHE FILEPATH "" )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=cortex-a8" )

add_definitions(-D__MAEMOFREMANTLE__ -D__MAEMO__)
