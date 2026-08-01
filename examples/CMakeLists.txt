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

if(BUILD_BLAM)
  add_subdirectory(blam/dimeter-info)
  add_subdirectory(blam/script-dumper)
endif()

# Trimmed BSP+lightmap viewer for the GX backend (no gleam/GLSL).
if(BUILD_BLAM AND BUILD_GX)
  add_subdirectory(blam/gx-bsp)
endif()

# RS2 cache terrain parser (Linux only — uses fork/exec bzip2)
if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
  add_subdirectory(rs/terrain)
endif()

