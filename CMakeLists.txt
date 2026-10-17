cmake_minimum_required(VERSION 4.0)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_SKIP_RPATH ON)

option(BUILD_ASSET_TOOLS "Build tools for packaging resources" OFF)
option(BUILD_BINARIES "Build binaries" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_LIBRARIES "Build libraries" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_UTILITIES "Build tools" ON)

option(BUILD_BLAM "Build Blam! data readers" ON)
option(BUILD_BLAM_DIMETER "Build Halo 2 (dimeter) readers and tools" ON)
option(BUILD_BLAM_ELEGY "Build Halo 3 (elegy) readers and tools" OFF)
option(BUILD_GNS "Build GameNetworkingSockets" ON)
option(BUILD_HEADFUL "Build with graphics/windowing support" ON)
option(BUILD_LIBOGC "Build libogc components (GameCube/Wii only)" OFF)
option(BUILD_MEDIA "Build with media decode/encode support" OFF)
option(BUILD_NET "Build with networking" ON)
option(BUILD_OPENAL "Build with OpenAL-soft audio" ON)
option(BUILD_OPENGL "Target OpenGL API" ON)
option(BUILD_SDL3 "Build with SDL3 graphics backend" ON)

include(CMakeDependentOption)
cmake_dependent_option(
  BUILD_OPENAL_WMA "Build the WMA decoder (LGPL)" ON
  "BUILD_BLAM;BUILD_BLAM_DIMETER;BUILD_OPENAL" OFF
)
cmake_dependent_option(
  BUILD_OPENAL_XMA2 "Build the XMA2 decoder on a shared libavcodec (LGPL)" ON
  "BUILD_BLAM;BUILD_BLAM_ELEGY;BUILD_OPENAL" OFF
)

if(BUILD_ASSET_TOOLS)
  list(APPEND VCPKG_MANIFEST_FEATURES pressure-cooker)
endif()
if(BUILD_BENCHMARKS)
  list(APPEND VCPKG_MANIFEST_FEATURES benchmarks)
endif()
if(BUILD_BLAM)
  list(APPEND VCPKG_MANIFEST_FEATURES blam)
endif()
if(BUILD_BLAM AND (BUILD_ASSET_TOOLS OR BUILD_EMSCRIPTEN))
  list(APPEND VCPKG_MANIFEST_FEATURES map-transcode)
endif()
if(BUILD_EXAMPLES)
  list(APPEND VCPKG_MANIFEST_FEATURES cursed)
endif()
if(BUILD_GNS)
  list(APPEND VCPKG_MANIFEST_FEATURES gns)
endif()
if(BUILD_LIBOGC)
  list(APPEND VCPKG_MANIFEST_FEATURES devkit)
endif()
if(BUILD_HEADFUL)
  list(APPEND VCPKG_MANIFEST_FEATURES headful)
endif()
if(BUILD_MEDIA)
  list(APPEND VCPKG_MANIFEST_FEATURES media)
endif()
if(BUILD_NET)
  list(APPEND VCPKG_MANIFEST_FEATURES network)
endif()
if(BUILD_OPENAL)
  list(APPEND VCPKG_MANIFEST_FEATURES audio)
endif()
if(BUILD_OPENAL_WMA)
  list(APPEND VCPKG_MANIFEST_FEATURES wma)
endif()
if(BUILD_OPENAL_XMA2)
  list(APPEND VCPKG_MANIFEST_FEATURES xma2)
endif()
if(BUILD_OPENGL)
  list(APPEND VCPKG_MANIFEST_FEATURES gleam)
endif()
if(BUILD_SDL3 AND NOT EMSCRIPTEN)
  list(APPEND VCPKG_MANIFEST_FEATURES sdl3)
endif()
if(BUILD_TESTS)
  list(APPEND VCPKG_MANIFEST_FEATURES tests)
endif()

project(Coffee LANGUAGES C CXX VERSION 4.0)

include(toolchain/cmake/Modules/SubprojectFunctions.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_CXX_SCAN_FOR_MODULES FALSE)

# glm SIMD for aligned_* types; default types stay packed for tag layouts
add_compile_definitions(GLM_FORCE_INTRINSICS)

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
