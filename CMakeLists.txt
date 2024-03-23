cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0069 NEW)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_SKIP_RPATH ON)

project(Coffee LANGUAGES C CXX VERSION 4.0)

include(toolchain/cmake/Modules/SubprojectFunctions.cmake)

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
