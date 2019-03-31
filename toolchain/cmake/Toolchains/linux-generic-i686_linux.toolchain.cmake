#
# This toolchain is used for cross-compiling 32-bit on 64-bit systems
# This is not used for native compiling!
#

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "i686" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32" CACHE STRING "c flags")
