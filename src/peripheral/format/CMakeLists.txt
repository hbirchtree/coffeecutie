find_package(fmt CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

coffee_library(
  TARGET PeripheralFormat
  SOURCES private/string_formatters.cpp
  LIBRARIES PeripheralLib fmt::fmt magic_enum::magic_enum
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(ANDROID AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
  target_compile_definitions(
    PeripheralFormat
    PUBLIC
    -D_LIBCPP_HAS_NO_OFF_T_FUNCTIONS
  )
endif()
