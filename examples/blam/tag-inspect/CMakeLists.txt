find_package(BCDec REQUIRED)

coffee_application(
  TARGET blam_tag_inspect
  TITLE "Blam Tag Inspect"
  SOURCES main.cpp
  LIBRARIES Core Blam Image GraphicsDefines
  USE_CMD
)

if(COFFEE_CPP_REFLECTION_AVAILABLE)
    set_target_properties(blam_tag_inspect
        PROPERTIES
        CXX_STANDARD 26
    )
    target_compile_options(blam_tag_inspect PRIVATE -freflection)
endif()

target_compile_definitions(blam_tag_inspect PRIVATE GLEAM_DUMMY)
target_include_directories(blam_tag_inspect PRIVATE ${BCDec_INCLUDE_DIR})
