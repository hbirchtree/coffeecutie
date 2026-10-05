coffee_application(
  TARGET MapTranscode
  TITLE "Map Bitmap Transcoder"
  SOURCES main.cpp
  LIBRARIES Core Blam BlamTranscode CoreZ GraphicsDefines
)

target_compile_definitions(MapTranscode PRIVATE
    -DGLEAM_DUMMY=1
)
