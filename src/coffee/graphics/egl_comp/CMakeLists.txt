option(
  BUILD_EGL_DOUBLE_GETPROC
  "Lookup GL functions in EGL *and* GLES, used as a workaround for odd EGL implementations, eg. ImgTec"
  OFF
)
option(BUILD_EGL_STATIC "Use EGL_static library, eg. for Raspberry Pi" OFF)
option(BUILD_EGL_DYNAMIC "Use GLAD for finding EGL functions" OFF)
option(BUILD_EGL_ANGLE "Use EGL_angle library" OFF)

coffee_library(
  TARGET EGLComponent
  SOURCES private/egl_comp.cpp
  LIBRARIES ComponentApp
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(BUILD_GLKIT)
  target_link_libraries(EGLComponent PUBLIC CEAGL)
  target_compile_definitions(EGLComponent PUBLIC -DEGL_USE_CEAGL)
elseif(BUILD_EGL_ANGLE)
  find_library(EGL_ANGLE_LIB EGL_angle)
  find_library(ANGLE_LIB ANGLE)
  target_link_libraries(EGLComponent PUBLIC GLAD ${EGL_ANGLE_LIB} ${ANGLE_LIB})
  target_compile_definitions(EGLComponent PUBLIC
    -DEGL_USE_STATIC
    -DEGL_USE_ANGLE
  )
elseif(BUILD_EGL_DYNAMIC)
  target_link_libraries(EGLComponent PUBLIC GLAD)
  target_compile_definitions(EGLComponent PUBLIC -DEGL_USE_DYNAMIC)
elseif(BUILD_EGL_STATIC)
  find_library(EGL_STATIC_LIB EGL_static)
  target_link_libraries(EGLComponent PUBLIC ${EGL_STATIC_LIB})
  target_compile_definitions(EGLComponent PUBLIC -DEGL_USE_STATIC)
else()
  find_library(EGL_LIB EGL)
  target_link_libraries(EGLComponent PUBLIC ${EGL_LIB})
  target_compile_definitions(EGLComponent PUBLIC -DEGL_USE_LINKED)
endif()

target_link_libraries(EGLComponent PRIVATE GraphicsDefines)

if(BUILD_EGL_DOUBLE_GETPROC)
  #
  # Workaround for ImTec's weird EGL loader It will not let you load the core
  # functions, only extensions Work around this by first looking up the symbol
  # in GLESv2, then EGL
  #
  target_compile_definitions(EGLComponent PUBLIC -DEGL_COMP_DOUBLE_GETPROC)
endif()
