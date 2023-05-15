cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0069 NEW)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

project(
  Coffee
  LANGUAGES C CXX
  VERSION 4.0
)

include(toolchain/cmake/Modules/SubprojectFunctions.cmake)

init_project()

include(GitDependencies)

set(COFFEE_EXT_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/src/libs")

set(COFFEE_ASSET_DIRECTORY "${CMAKE_SOURCE_DIR}/sample_data")

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

  if(BUILD_TESTS)
    option(SKIP_HIGHMEM_TESTS "Skip high-memory tests" OFF)
    option(SKIP_LINKAGE_TEST "Skip test of dynamic linking" ON)
    option(SKIP_OPENVR_TEST
           "Skip tests of OpenVR, it requires a display and hardware" ON
    )
    option(SKIP_GRAPHIC_TESTS "Skip tests depending on headful configuration"
           ON
    )

    add_subdirectory(tests)
  endif()
endif()

# Add documentation directory, not built by default (increases build time)
add_subdirectory(toolchain/docs-gen)

end_project()
