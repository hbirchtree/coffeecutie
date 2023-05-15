find_package(zstd CONFIG REQUIRED)

coffee_library(
  TARGET CoreZstd
  SOURCES private/zstd.cpp
  LIBRARIES zstd::libzstd_static PeripheralLib
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              HEADER_BASE corezstd
)
