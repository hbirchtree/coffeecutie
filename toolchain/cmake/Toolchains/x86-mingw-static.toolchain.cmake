set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)
set(CMAKE_CROSSCOMPILING ON)

set(CMAKE_C_COMPILER i686-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++-posix)

add_compile_definitions(
  -D__MINGW64__
  -D_WIN32_WINNT=0x0602
)
add_compile_options(
  -Wa,-mbig-obj
)
add_link_options(
  -Wa,-mbig-obj
)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wa,-mbig-obj")
