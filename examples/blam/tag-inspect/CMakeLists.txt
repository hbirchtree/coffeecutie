find_package(BCDec REQUIRED)

coffee_application(
  TARGET blam_tag_inspect
  TITLE "Blam Tag Inspect"
  SOURCES main.cpp
  LIBRARIES Core Blam Image GraphicsDefines
  USE_CMD
)

target_compile_definitions(blam_tag_inspect PRIVATE GLEAM_DUMMY)
target_include_directories(blam_tag_inspect PRIVATE ${BCDec_INCLUDE_DIR})
