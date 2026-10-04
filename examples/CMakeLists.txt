#
# Here lies examples of code usage
#

add_subdirectory(core/env-test)

if(BUILD_GLEAM OR BUILD_GX)
  add_subdirectory(gleam/rhi/base-test)
endif()

if(BUILD_BLAM AND BUILD_GLEAM)
  add_subdirectory(blam/cblam-testing)
  add_subdirectory(blam/cblam-pose-demo)
endif()

if(BUILD_BLAM AND NOT (
    ANDROID OR
    APPLE OR
    EMSCRIPTEN OR
    IOS OR
    GAMECUBE OR
    WII)
)
  # Apple's Clang yells at tag-inspect
  add_subdirectory(blam/dimeter-info)
  add_subdirectory(blam/map-decompress)
  add_subdirectory(blam/script-dumper)
  add_subdirectory(blam/tag-inspect)
endif()

# Stores maps in the browser for the web builds of BlamGraphics
if(BUILD_BLAM AND EMSCRIPTEN AND CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_subdirectory(blam/map-upload)
endif()

# Trimmed BSP+lightmap viewer for the GX backend (no gleam/GLSL).
if(BUILD_BLAM AND BUILD_GX)
  add_subdirectory(blam/gx-bsp)
endif()

# RS2 cache terrain parser (Linux only — uses fork/exec bzip2)
if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
  add_subdirectory(rs/terrain)
endif()

