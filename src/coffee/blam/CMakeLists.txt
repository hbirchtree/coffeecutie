coffee_library(
  TARGET Blam
  SOURCES private/volta/blam_bitm.cpp private/volta/blam_map.cpp
  LIBRARIES Image CoreZ
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)

target_compile_options(Blam PUBLIC
    # We're doing some checks for the sizes of structs, so ignore the warnings...
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-invalid-offsetof>
)

#target_precompile_headers(Blam PUBLIC $<$<COMPILE_LANGUAGE:CXX>:blam/cblam.h>)
target_link_libraries(Blam PRIVATE GraphicsDefines)
