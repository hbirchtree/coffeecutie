coffee_application(
  TARGET GLeamBaseTest_RHI
  TITLE "GLeam Basic RHI"
  SOURCES main.cpp renderer.h
  LIBRARIES Core CoreSaving EntComp ComponentBundleSetup
  RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/basic_test
)

# Select the RHI backend: GLeam (GL/GLES) where available, otherwise the GX
# (Gexxo) backend on Gekko. Linking the target auto-defines FEATURE_ENABLE_<lib>,
# which renderer.h uses to typedef rhi = gleam|gexxo.
if(TARGET GLeamRHI)
  target_link_libraries(GLeamBaseTest_RHI PUBLIC GLeamRHI)
elseif(TARGET Gexxo)
  target_link_libraries(GLeamBaseTest_RHI PUBLIC Gexxo)
endif()

set_target_properties(GLeamBaseTest_RHI PROPERTIES EMSCRIPTEN_MAX_HEAP_SIZE 20M)

if(TARGET GpuQuery)
  target_link_libraries(GLeamBaseTest_RHI PUBLIC GpuQuery)
endif()

if(TARGET Net)
  target_link_libraries(GLeamBaseTest_RHI PUBLIC Net)
  if(TARGET DiscordLatte)
    target_link_libraries(GLeamBaseTest_RHI PUBLIC DiscordLatte)
  endif()
endif()
