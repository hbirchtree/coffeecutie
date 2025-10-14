cmake_minimum_required(VERSION 4.0)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_SKIP_RPATH ON)

project(Coffee LANGUAGES C CXX VERSION 4.0)

include(toolchain/cmake/Modules/SubprojectFunctions.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(NOT IOS AND NOT MINGW)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT LTOSupported)

  if(LTOSupported)
    set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  endif()
endif()

init_project()


if(BUILD_LIBRARIES)
  add_subdirectory(src)
endif()

if(BUILD_LIBRARIES AND BUILD_BINARIES)
  # Tools, includes the GUI profiler
  if(BUILD_UTILITIES)
    add_subdirectory(tools)
  endif()

  # Add subdir for example applications
  if(BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif()
endif()

end_project()
