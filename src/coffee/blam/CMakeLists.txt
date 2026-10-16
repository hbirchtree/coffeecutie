# blam_bitm.cpp converts bitmaps to GL texture formats (glw) and CoreZ/Image are
# for compression + PNG/JPG -- none needed on bare consoles, where we only parse
# uncompressed PC maps + read raw (e.g. RGB565) pixels. Drop them there.
set(BLAM_SOURCES
  private/volta/blam_map.cpp
  private/volta/blam_bytecode.cpp
  private/dimeter/script_opcodes.cpp
)
set(BLAM_LIBS Core) # result/semantic/task_queue (got via Image transitively before)
if(NOT (GAMECUBE OR WII))
  list(APPEND BLAM_SOURCES private/volta/blam_bitm.cpp)
  list(APPEND BLAM_LIBS Image CoreZ GLTextureFormats)
  if(BUILD_BLAM_ELEGY)
    list(APPEND BLAM_SOURCES private/elegy/resources.cpp)
  endif()
  if(BUILD_OPENAL_XMA2)
    list(APPEND BLAM_SOURCES private/elegy/xma2.cpp)
  endif()
endif()

coffee_library(
  TARGET Blam
  SOURCES ${BLAM_SOURCES}
  LIBRARIES ${BLAM_LIBS}
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)

if(BUILD_BLAM_ELEGY AND NOT (GAMECUBE OR WII))
  target_compile_definitions(Blam PUBLIC BLAM_ELEGY=1)
endif()
if(BUILD_OPENAL_XMA2 AND NOT (GAMECUBE OR WII))
  # Shared libavcodec only: the XMA2 decoder is LGPL
  find_package(FFMPEG REQUIRED)
  target_include_directories(Blam PRIVATE ${FFMPEG_INCLUDE_DIRS})
  target_link_libraries(Blam PRIVATE ${FFMPEG_LIBRARIES})
  target_compile_definitions(Blam PUBLIC BLAM_ELEGY_XMA2=1)
endif()

target_compile_options(Blam PUBLIC
    # We're doing some checks for the sizes of structs, so ignore the warnings...
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-invalid-offsetof>
)

#target_precompile_headers(Blam PUBLIC $<$<COMPILE_LANGUAGE:CXX>:blam/cblam.h>)
if(NOT (GAMECUBE OR WII))
  target_link_libraries(Blam PRIVATE GraphicsDefines)
endif()

if(BUILD_ASSET_TOOLS OR EMSCRIPTEN)
  add_subdirectory(transcode)
endif()
