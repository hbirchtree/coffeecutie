coffee_library(
  TARGET SDL3Components
  SOURCES private/sdl3_components.cpp
  LIBRARIES ComponentApp
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(EMSCRIPTEN)
  target_compile_options(SDL3Components PUBLIC -sUSE_SDL=3 -Wno-experimental)
  target_link_options(SDL3Components PUBLIC -sUSE_SDL=3 -Wno-experimental)
else()
  find_package(SDL3 CONFIG REQUIRED)
  target_link_libraries(SDL3Components PUBLIC SDL3::SDL3)
endif()

if(RASPBERRY)
  target_link_libraries(
    SDL3Components PUBLIC bcm_host vchiq_arm vcos EGL GLESv2
  )
endif()
