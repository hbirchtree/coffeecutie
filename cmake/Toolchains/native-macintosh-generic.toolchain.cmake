# Temporary workaround
include_directories ( "/usr/local/opt/llvm38/lib/llvm-3.8/include/c++/v1" )
link_directories("/usr/local/opt/llvm38/lib/llvm-3.8/lib")
# Forcing use of C++11 (or later) libc++
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11" )
# To prevent undefined symbol errors when creating OSX frameworks
# License information and application information is undefined in the library for a reason
set ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_LD_FLAGS} -Wl,-undefined,dynamic_lookup" )
