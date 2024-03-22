set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CROSSCOMPILING ON)

include(${CMAKE_CURRENT_LIST_DIR}/common/toolchain-prefix.cmake)

set(CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}gcc" CACHE FILEPATH "")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}g++" CACHE FILEPATH "")
set(CMAKE_RC_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}windres" CACHE FILEPATH "")
set(CMAKE_DLLTOOL_COMPILER "${TOOLCHAIN_ROOT}/bin/${TOOLCHAIN_TOOL}dlltool" CACHE FILEPATH "")

add_compile_definitions(
  __MINGW64__
  _WIN32_WINNT=0x0602
)
add_compile_options(
  -Wa,-mbig-obj
)
add_link_options(
  -Wa,-mbig-obj
)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wa,-mbig-obj")

include(${CMAKE_CURRENT_LIST_DIR}/common/configure-paths.cmake)
