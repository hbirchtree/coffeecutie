find_package(fmt CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

coffee_library(
  TARGET PeripheralFormat
  SOURCES private/string_formatters.cpp
  LIBRARIES PeripheralLib fmt::fmt magic_enum::magic_enum
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
