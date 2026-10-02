coffee_application(
  TARGET blam_map_decompress
  TITLE "Blam Map Decompressor"
  SOURCES main.cpp
  LIBRARIES Core Blam GraphicsDefines
  USE_CMD
)

target_compile_definitions(blam_map_decompress PRIVATE GLEAM_DUMMY)
