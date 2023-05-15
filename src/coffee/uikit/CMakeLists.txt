coffee_library(
  TARGET UIKitGestures
  SOURCES private/uikit_gestures.mm
  LIBRARIES ComponentApp "-framework GameController"
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
