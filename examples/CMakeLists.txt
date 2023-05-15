#
# Here lies examples of code usage
#

add_subdirectory(core/env-test)

if(BUILD_GLEAM)
  add_subdirectory(gleam/rhi/base-test)
endif()

if(BUILD_BLAM AND BUILD_GLEAM)
  add_subdirectory(blam/cblam-testing)
endif()
