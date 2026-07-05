coffee_application(
  TARGET MapTranscode
  TITLE "Map Bitmap Transcoder"
  SOURCES main.cpp
  LIBRARIES Core Blam GraphicsDefines
)

# Reads the GX-native auxiliary format codes (native_format.h) as header-only;
# no link against the GX backend (this is a host tool, no libogc).
target_compile_definitions(MapTranscode PRIVATE
    -DGLEAM_DUMMY=1
)
find_package(BCDec REQUIRED)
target_include_directories(MapTranscode PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/coffee/graphics/apis/gx/include
  ${BCDec_INCLUDE_DIR}
)
find_package(unofficial-pvrtc CONFIG REQUIRED)
target_link_libraries(MapTranscode PRIVATE unofficial::pvrtc::pvrtc)

find_package(Etc2Comp CONFIG REQUIRED)
target_link_libraries(MapTranscode PRIVATE Etc2Comp::EtcLib)
