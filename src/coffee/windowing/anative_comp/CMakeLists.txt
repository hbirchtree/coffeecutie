set(ANDROID_SOURCES_DIR "${ANDROID_NDK}/sources/android")

# coffee_library(
#   TARGET ANativeDependencies
#   SOURCES ${ANDROID_SOURCES_DIR}/ndk_helper/gestureDetector.cpp
#   HEADER_DIRS $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/ndk_helper>
#               $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/native_app_glue>
# )

coffee_library(
  TARGET ANativeComponent
  SOURCES
    private/anative_comp.cpp
    private/gestureDetector.cpp
    private/vecmath.cpp
  LIBRARIES ComponentApp #[[ANativeDependencies]]
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<BUILD_INTERFACE:${ANDROID_SOURCES_DIR}/native_app_glue>
              $<INSTALL_INTERFACE:include>
)
