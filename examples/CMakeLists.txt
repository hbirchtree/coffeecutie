#
# Here lies examples of code usage
#

add_subdirectory(core/env-test)

if(BUILD_GLEAM OR BUILD_GX)
  add_subdirectory(gleam/rhi/base-test)
endif()

if(BUILD_BLAM AND BUILD_GLEAM)
  add_subdirectory(blam/cblam-testing)
endif()

# Trimmed BSP+lightmap viewer for the GX backend (no gleam/GLSL).
if(BUILD_BLAM AND BUILD_GX)
  add_subdirectory(blam/gx-bsp)
endif()
