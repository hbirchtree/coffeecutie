set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc-eabi)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_EXECUTABLE_SUFFIX "elf")

set(GAMECUBE ON CACHE BOOL "")
add_definitions(-DGEKKO -D__GEKKO__ -D__GEKKO_RVL__)

set(CMAKE_C_FLAGS "-mrvl -mcpu=750 -meabi -mhard-float" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")

include(${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake)

set(CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++")

include(${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake)
