coffee_library(
  TARGET OSMesaComponent
  SOURCES private/osmesa_comp.cpp
  LIBRARIES ComponentApp Image OSMesa
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)
