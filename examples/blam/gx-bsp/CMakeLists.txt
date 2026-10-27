coffee_application(
  TARGET BlamGX
  TITLE "Blam BSP (GX)"
  SOURCES main.cpp
  LIBRARIES Core EntComp ComponentBundleSetup Blam
  RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

# GX (Gexxo) backend on Gekko for video bring-up + OSReport logging + the paged
# mmap / dvd: devoptab the map loader uses.
if(TARGET Gexxo)
  target_link_libraries(BlamGX PUBLIC Gexxo)
endif()

# Network play goes through BlamGCGateway over the broadband adapter
if(GAMECUBE)
  find_package(libogc CONFIG REQUIRED)
  target_link_libraries(BlamGX PRIVATE libogc::bba)
  target_include_directories(BlamGX PRIVATE
      ${PROJECT_SOURCE_DIR}/tools/blam/gc-gateway/include
      # BlamGraphics' message layouts, network/network_events.h
      ${PROJECT_SOURCE_DIR}/examples/blam/cblam-testing
  )
endif()
