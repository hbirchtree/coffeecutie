find_package(Vorbis CONFIG REQUIRED)

coffee_library(
  TARGET OggDecoder
  SOURCES private/ogg_decode.cpp
  LIBRARIES Core Vorbis::vorbisfile
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

coffee_library(
  TARGET OAF
  SOURCES private/api.cpp private/wav_decode.cpp
  LIBRARIES ComponentApp OggDecoder
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(EMSCRIPTEN AND FALSE)
  find_package(Microsoft.GSL CONFIG REQUIRED)
  coffee_library(
    TARGET OpenAL_ImaDecoder
    LINKAGE SHARED
    SOURCES
      private/ima_decode.cpp
    LIBRARIES Microsoft.GSL::GSL
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_link_options(OpenAL_ImaDecoder PUBLIC -pthread)
  target_compile_definitions(OAF PUBLIC
    -DOAF_IMA_DECODER_ENABLED=1
  )
  target_link_libraries(OAF PUBLIC
    ${CMAKE_BINARY_DIR}/lib/libOpenAL_ImaDecoder.wasm
  )
  add_dependencies(OAF OpenAL_ImaDecoder)
endif()

if(ANDROID)
  # OpenSL is a dependency for OpenAL
  find_package(OpenSL REQUIRED)
endif()
find_package(OpenAL CONFIG REQUIRED)
target_link_libraries(OAF PUBLIC OpenAL::OpenAL)

# WMA through the libwma port (shared library, not on every platform)
find_package(libwma CONFIG QUIET)
if(TARGET libwma::wma)
  target_sources(OAF PRIVATE private/wma_decode.cpp)
  target_link_libraries(OAF PUBLIC libwma::wma)
  target_compile_definitions(OAF PUBLIC -DOAF_WMA_DECODER_ENABLED=1)
endif()

# XMA2 through libavcodec: LGPL, so only ever a shared library
if(BUILD_OPENAL_XMA2)
  find_package(FFMPEG REQUIRED)
  target_sources(OAF PRIVATE private/xma2_decode.cpp)
  target_include_directories(OAF PRIVATE ${FFMPEG_INCLUDE_DIRS})
  target_link_libraries(OAF PUBLIC ${FFMPEG_LIBRARIES})
  target_compile_definitions(OAF PUBLIC -DOAF_XMA2_DECODER_ENABLED=1)
endif()

if(EMSCRIPTEN)
  # openal-soft is a side module, mixed into WebAudio by web_output.cpp
  target_sources(OAF PRIVATE private/web_output.cpp)
  # Emscripten's libopenal.js would shadow the side module's AL functions,
  # so link the default JS libraries without it
  target_link_options(OAF PUBLIC
    -sAUTO_JS_LIBRARIES=0
    -lwebgl.js -lglut.js -lxlib.js -legl.js -luuid.js -lglew.js
    -lidbstore.js -lasync.js -lsdl.js
  )
endif()
