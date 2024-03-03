add_library(GraphicsAPI INTERFACE)

target_include_directories(GraphicsAPI INTERFACE $<INSTALL_INTERFACE:include>)

header_install(
  GraphicsAPI $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include> ""
)

add_export(GraphicsAPI "")

add_library(GraphicsInterface INTERFACE)

target_include_directories(
  GraphicsInterface
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

coffee_bundle(
  DESTINATION "" HEADER_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_export(GraphicsInterface "")

