# Bitmap transcoders behind MapTranscode and BlamMapUpload
coffee_library(
  TARGET BlamTranscode
  LINKAGE STATIC
  SOURCES private/kernels.cpp private/map_transcode.cpp
  LIBRARIES Blam
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  NO_EXPORT
)

target_compile_definitions(BlamTranscode PRIVATE GLEAM_DUMMY=1)

find_package(BCDec REQUIRED)
target_include_directories(BlamTranscode PRIVATE ${BCDec_INCLUDE_DIR})
find_package(unofficial-pvrtc CONFIG REQUIRED)
find_package(Etc2Comp CONFIG REQUIRED)
target_link_libraries(BlamTranscode PRIVATE
  GraphicsDefines
  unofficial::pvrtc::pvrtc
  Etc2Comp::EtcLib
)
