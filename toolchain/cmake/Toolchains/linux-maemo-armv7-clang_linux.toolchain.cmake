set ( CMAKE_SYSTEM_NAME Linux )
set ( CMAKE_SYSTEM_PROCESSOR arm )
set ( TOOLCHAIN_PREFIX "arm-linux-gnueabi" )

set ( CMAKE_SYSROOT "/home/havard/Code/Maemo5/maemo-root" )
#set ( CMAKE_SYSROOT /usr/${TOOLCHAIN_PREFIX} )

#set ( GCC_ROOT "${CMAKE_SYSROOT}/opt/gcc-6.1" )'

set ( MAEMO TRUE CACHE BOOL "" )

set ( MAEMO_SDK "${MAEMO_SDK}" CACHE PATH "" )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc-5" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++-5" )

set ( CMAKE_C_COMPILER_TARGET ${TOOLCHAIN_PREFIX} )
set ( CMAKE_CXX_COMPILER_TARGET ${TOOLCHAIN_PREFIX} )

set ( CMAKE_RANLIB "${TOOLCHAIN_PREFIX}-ranlib" CACHE FILEPATH "" )
set ( CMAKE_AR "${TOOLCHAIN_PREFIX}-ar" CACHE FILEPATH "" )
set ( CMAKE_LINKER "${TOOLCHAIN_PREFIX}-ld" CACHE FILEPATH "" )

set ( CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -mfpu=vfp -mfloat-abi=soft -mtune=cortex-a8"
    CACHE STRING "" )

add_definitions(
    -D__MAEMOFREMANTLE__
    -D__MAEMO__
#    -D__linux__
#    -D__unix__
    )

link_libraries (
#    c
#    rt
    -static-libstdc++
    -static-libgcc
#    atomic
    )

include_directories (
#    /usr/arm-linux-gnueabi/include/c++/5/arm-linux-gnueabi/
#    /usr/arm-linux-gnueabi/include/c++/5/
#    /usr/arm-linux-gnueabi/include
    )
