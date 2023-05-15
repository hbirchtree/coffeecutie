find_package(imgui CONFIG REQUIRED)

coffee_library(
  TARGET ImGui
  SOURCES private/imgui_binding.cpp private/imgui_gleam.cpp
          private/imgui_input.cpp private/imgui_widgets.cpp
  LIBRARIES ComponentBundleSetup GLeamRHI imgui::imgui
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)
target_compile_definitions(
  ImGui
  PUBLIC -DCOFFEE_IMGUI_USE_GLEAM -DIMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS
         -DIMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS
)
target_compile_definitions(ImGui PRIVATE -DIM_API="ImGui::")
