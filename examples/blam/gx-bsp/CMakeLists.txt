coffee_application(
  TARGET BlamBSP_GX
  TITLE "Blam BSP (GX)"
  SOURCES main.cpp
  LIBRARIES Core EntComp ComponentBundleSetup Blam
)

# GX (Gexxo) backend on Gekko for video bring-up + OSReport logging + the paged
# mmap / dvd: devoptab the map loader uses.
if(TARGET Gexxo)
  target_link_libraries(BlamBSP_GX PUBLIC Gexxo)
endif()
