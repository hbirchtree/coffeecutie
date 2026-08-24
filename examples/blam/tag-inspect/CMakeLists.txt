coffee_application(
  TARGET blam_tag_inspect
  TITLE "Blam Tag Inspect"
  SOURCES main.cpp
  LIBRARIES Core Blam GraphicsDefines
  USE_CMD
)

target_compile_definitions(blam_tag_inspect PRIVATE GLEAM_DUMMY)
