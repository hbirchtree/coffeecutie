set(CMAKE_SYSTEM_NAME Baremetal)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR powerpc)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_EXECUTABLE_SUFFIX "elf")

set(WII ON CACHE BOOL "")
set(RVL ON CACHE BOOL "")

include(${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake)

set(CMAKE_SYSROOT ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX} CACHE PATH "")

include_directories(AFTER SYSTEM
    ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}/sys-include
    ${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}/include
    )

set(CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++")

set(GAMECUBE_MACHINE_FLAGS
  "-mrvl -mcpu=750 -meabi -mhard-float -DHW_RVL -DBIGENDIAN -DGEKKO -D__GEKKO__ -D_HAVE_LONG_DOUBLE -D_LDBL_EQ_DBL"
)
# gcc 14+ promotes these to hard errors by default; older C dependencies
# (e.g. zstd's legacy decoders) still trip them, so demote back to warnings.
set(CMAKE_C_FLAGS
    "${GAMECUBE_MACHINE_FLAGS} -Wno-error=incompatible-pointer-types -Wno-error=implicit-function-declaration -Wno-error=int-conversion"
    CACHE STRING "")
set(CMAKE_CXX_FLAGS "${GAMECUBE_MACHINE_FLAGS}" CACHE STRING "")
# Assembly (.S) needs the same machine flags. -mregnames lets the assembler
# accept register names (r3/sp/...) used by libogc's hand-written assembly;
# _LANGUAGE_ASSEMBLY keeps the older asm.h register-alias path working too.
set(CMAKE_ASM_FLAGS "${GAMECUBE_MACHINE_FLAGS} -mregnames -D_LANGUAGE_ASSEMBLY" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "-L${TOOLCHAIN_ROOT}/${TOOLCHAIN_PREFIX}/lib -lnosys -lsysbase -lm" CACHE STRING "")

include(${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake)
