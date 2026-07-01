coffee_application(
  TARGET MapTranscode
  TITLE "Map Bitmap Transcoder"
  SOURCES main.cpp
  LIBRARIES Core Blam
)

# Reads the GX-native auxiliary format codes (native_format.h) as header-only;
# no link against the GX backend (this is a host tool, no libogc).
target_include_directories(MapTranscode PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/coffee/graphics/apis/gx/include)
