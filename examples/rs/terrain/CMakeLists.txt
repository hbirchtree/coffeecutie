find_package(BZip2 REQUIRED)

coffee_application(
  TARGET RSTerrainParser
  TITLE "RS2 Terrain Parser"
  SOURCES main.cpp
  LIBRARIES Core CoreZ
)

# Expose bundled zlib headers so we can call inflateInit2 directly for raw deflate
target_include_directories(RSTerrainParser PRIVATE
  "${CMAKE_SOURCE_DIR}/src/peripheral/compressors/zlib/impl/include"
)
# JAG archive bodies are headerless bzip2 streams
target_link_libraries(RSTerrainParser PRIVATE BZip2::BZip2)
