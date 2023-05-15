coffee_library(
  TARGET MediaFfmpeg
  SOURCES private/interface.cpp
  LIBRARIES Core
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

find_package(FFMPEG REQUIRED)

target_link_libraries(MediaFfmpeg PUBLIC ${FFMPEG_LIBRARIES} # vdpau va
)
target_include_directories(
  MediaFfmpeg PUBLIC $<BUILD_INTERFACE:${FFMPEG_INCLUDE_DIRS}>
)
