option(BUILD_COMP_DUMMY_PLUG "Build dummy plug support in ComponentApp" ON)

coffee_library(
  TARGET ComponentApp
  SOURCES
    private/comp_app.cpp
    private/file_mapper.cpp
    private/file_watcher.cpp
    private/fps_counter.cpp
    private/subsystems.cpp
  LIBRARIES EntComp Core
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bundle_include>
)

#target_precompile_headers(
#  ComponentApp PUBLIC $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/subsystems.h>
#)

coffee_library(
  TARGET ComponentBundleSetup
  SOURCES
    private/bundle.cpp
  LIBRARIES ComponentApp ComponentBundle GraphicsBundle MobileBundle Image
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bundle_include>
)

if(BUILD_COMP_DUMMY_PLUG)
  target_sources(ComponentBundleSetup PUBLIC
    private/dummy_plug.cpp
  )
  target_compile_definitions(ComponentBundleSetup PUBLIC
    -DFEATURE_ENABLE_ComponentBundleSetup_DummyPlug=1
  )
endif()

#target_precompile_headers(
#  ComponentBundleSetup
#  PUBLIC
#  $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/bundle.h>
#  $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/app_wrap.h>
#  $<$<COMPILE_LANGUAGE:CXX>:coffee/comp_app/AppWrap>
#)
