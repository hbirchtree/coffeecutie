#
# Google Native Client toolchain file for CMake
#
# (c) Copyrights 2009-2011 Hartmut Seichter
# 
# Note: only works on OSX
#

set(NACL TRUE CACHE BOOL "")

set(NACL_PATH $ENV{NACL_ROOT} CACHE STRING "Native Client SDK Root Path")

set(NACL_TAG pnacl)

set(NACL_HOST linux_pnacl )

set( CMAKE_EXECUTABLE_SUFFIX ".pexe" CACHE STRING "" )
set( CMAKE_EXECUTABLE_SUFFIX_CXX ".pexe" CACHE STRING "" )

if(CMAKE_TOOLCHAIN_FILE)
    # Stub the warning
endif()

add_definitions(-D__NATIVE_CLIENT__)

# 
#
# 
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

#set(CMAKE_SYSTEM_PROCESSOR arm-eabi)

set(CMAKE_FIND_ROOT_PATH
	${NACL_PATH}/toolchain/${NACL_HOST}/nacl
	)


set(CMAKE_ASM_COMPILER ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-as )
set(CMAKE_C_COMPILER   ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-clang )
set(CMAKE_CXX_COMPILER ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-clang++ )
set(CMAKE_AR ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-ar CACHE FILEPATH "")
set(CMAKE_NM ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-nm CACHE FILEPATH "")
set(CMAKE_RANLIB ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-ranlib CACHE FILEPATH "")
set(CMAKE_LINKER ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-ld CACHE FILEPATH "")

set(NACL_FINALIZE ${NACL_PATH}/toolchain/${NACL_HOST}/bin/${NACL_TAG}-finalize CACHE FILEPATH "")

set(CMAKE_C_FLAGS 
#	"--prefix=${NACL_PATH}/cross/armv7a --exec_prefix=${NACL_PATH}/cross/armv7a #--with-mpfr=${NACL_PATH}/staging/i686-linux/usr"
        CACHE STRING "OpenEmbedded - GCC/C flags" FORCE
)

set(CMAKE_CXX_FLAGS 
        "-std=gnu++14"
        CACHE STRING "OpenEmbedded - GCC/C++ flags" FORCE
)

include_directories (
    "${NACL_PATH}/include"
    "${NACL_PATH}/include/pnacl"
    )

link_directories (
    "${NACL_PATH}/lib/pnacl/Release/"
    )

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
