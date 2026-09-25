if(NOT EMSCRIPTEN AND NOT APPLE)
  find_package(ZLIB)
endif()

coffee_library(
  TARGET CoreZ
  SOURCES private/corez.cpp
  LIBRARIES PeripheralLib
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              HEADER_BASE corez
)

if(TARGET ZLIB::ZLIB
   AND NOT EMSCRIPTEN
   AND NOT APPLE
)
  target_link_libraries(CoreZ PUBLIC ZLIB::ZLIB)
elseif(EMSCRIPTEN)
  target_compile_options(CoreZ PUBLIC -sUSE_ZLIB=1)
  target_link_options(CoreZ PUBLIC -sUSE_ZLIB=1)
elseif(ANDROID OR APPLE)
  target_link_libraries(CoreZ PUBLIC z)
endif()
