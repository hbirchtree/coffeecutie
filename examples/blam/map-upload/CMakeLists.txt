# Browser tool that stores Halo CE maps in IndexedDB for the web builds of
# BlamGraphics, see shell.html. The page drives the exported functions in
# main.cpp with plain numbers as pointers, which only holds on wasm32.
coffee_application(
  TARGET BlamMapUpload
  TITLE "Blam Map Upload"
  SOURCES main.cpp
  LIBRARIES Core Blam BlamTranscode CoreZ GraphicsDefines
  ICON_ASSET ${CMAKE_CURRENT_SOURCE_DIR}/../cblam-testing/icon_asset_plain.svg
  SHELL ${CMAKE_CURRENT_SOURCE_DIR}/shell.html
  USE_CMD
)

target_compile_definitions(BlamMapUpload PRIVATE GLEAM_DUMMY)

# Decompressed Xbox maps are up to a few hundred MB, held next to the input
set_target_properties(
  BlamMapUpload
  PROPERTIES EMSCRIPTEN_GROWABLE_MEMORY ON
)
target_link_options(BlamMapUpload PRIVATE -sMAXIMUM_MEMORY=2GB)
set_property(
  TARGET BlamMapUpload APPEND
  PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shell.html
)

# Fira Code and the noise texture behind the "plasma" hover, from the blog's
# forerunner theme the page is styled after
add_custom_command(
  TARGET BlamMapUpload
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/assets
      $<TARGET_FILE_DIR:BlamMapUpload>
)
