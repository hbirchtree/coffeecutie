coffee_application(
  TARGET blam_script_dumper
  TITLE "Blam Script Dumper"
  SOURCES main.cpp
  LIBRARIES Core Blam GraphicsDefines
  USE_CMD
)

target_compile_definitions(blam_script_dumper PRIVATE GLEAM_DUMMY)
