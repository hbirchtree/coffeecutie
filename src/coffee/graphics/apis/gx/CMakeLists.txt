option(BUILD_GX2 "Target GX2 API" OFF)

find_package(libogc CONFIG REQUIRED)

coffee_library(
  TARGET Gexxo
  SOURCES
    private/gexxo_video.cpp
    private/gexxo_rhi.cpp
  LIBRARIES Core Image ComponentApp GraphicsAPI GraphicsInterface
            libogc::libogc
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)
