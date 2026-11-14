find_package(BCDec REQUIRED)

coffee_application(
  TARGET VoltaInfo
  TITLE "Halo 1 Map Info"
  SOURCES main.cpp
  LIBRARIES Core Blam Image GraphicsDefines
  USE_CMD
)

if(COFFEE_CPP_REFLECTION_AVAILABLE)
    set_target_properties(VoltaInfo
        PROPERTIES
        CXX_STANDARD 26
    )
    target_compile_options(VoltaInfo PRIVATE -freflection)
endif()

target_compile_definitions(VoltaInfo PRIVATE GLEAM_DUMMY)
target_include_directories(VoltaInfo PRIVATE ${BCDec_INCLUDE_DIR})
