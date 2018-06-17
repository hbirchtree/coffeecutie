#set ( CLANG_PATH "/usr/local/opt/llvm@3.8" )
#set ( CMAKE_C_COMPILER ${CLANG_PATH}/bin/clang-3.8  )
#set ( CMAKE_CXX_COMPILER ${CLANG_PATH}/bin/clang++-3.8 )

#include_directories ( "${CLANG_PATH}/lib/llvm-3.8/include/c++/v1" )
#link_directories("${CLANG_PATH}/lib/llvm-3.8/lib")

#set ( CMAKE_C_COMPILER "$ENV{CC}"  )
#set ( CMAKE_CXX_COMPILER "$ENV{CXX}" )

# Forcing use of C++11 (or later) libc++
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE STRING "" )

set ( CMAKE_LIBRARY_ARCHITECTURE "osx-universal" CACHE STRING "" )
