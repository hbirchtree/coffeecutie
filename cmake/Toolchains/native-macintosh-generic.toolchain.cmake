set ( CLANG_PATH "/usr/local/opt/llvm@3.8" )
set ( CMAKE_C_COMPILER ${CLANG_PATH}/bin/clang-3.8  )
set ( CMAKE_CXX_COMPILER ${CLANG_PATH}/bin/clang++-3.8 )

include_directories ( "${CLANG_PATH}/lib/llvm-3.8/include/c++/v1" )
link_directories("${CLANG_PATH}/lib/llvm-3.8/lib")

set ( CMAKE_C_COMPILER "$ENV{CC}"  )
set ( CMAKE_CXX_COMPILER "$ENV{CXX}" )

# Forcing use of C++11 (or later) libc++
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11" CACHE STRING "" )
# To prevent undefined symbol errors when creating OSX frameworks
# License information and application information is undefined in the library for a reason
set ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_LD_FLAGS} -Wl,-undefined,dynamic_lookup" )
