coffee_library(
  TARGET CEAGL
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ceagl/private/eagl_bind.mm
          ${CMAKE_CURRENT_SOURCE_DIR}/ceagl/private/EGLView.m
  LIBRARIES PeripheralPlatform "-framework GLKit"
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ceagl/include>
)

target_compile_definitions(CEAGL PRIVATE -DGLES_SILENCE_DEPRECATION)

coffee_library(
  TARGET GLKitComponent
  SOURCES private/glk_comp.mm
  LIBRARIES ComponentApp CEAGL GLeamCommon_ES
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
