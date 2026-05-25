coffee_library(
  TARGET TerminalComponents
  SOURCES private/terminal_components.cpp
  LIBRARIES ComponentApp
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
