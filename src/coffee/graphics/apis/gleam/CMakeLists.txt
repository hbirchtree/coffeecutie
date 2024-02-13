option(BUILD_GLES_20 "Build against OpenGL ES 2.0 (mutually exclusive)" OFF)
option(BUILD_GLES_20_STATIC
       "Build against OpenGL ES 2.0 (linked, no extensions)" OFF
)
option(BUILD_GLES "Build library targeting OpenGL ES 3.0+" OFF)
option(
  BUILD_GLES_31
  "Build with support for OpenGL ES 3.1 (Only activates when BUILD_GLES is on)"
  ON
)
option(
  BUILD_GLES_32
  "Build with support for OpenGL ES 3.2 (Only activates when BUILD_GLES is on)"
  ON
)

option(BUILD_GL_SOFTWARE_BCN "Build software-decode for BCn using bcdec" OFF)
option(BUILD_GL_SOFTWARE_PVRTC "Build software-decode for PVRTC" OFF)

if(WIN32 AND WIN_UWP)
  option(BUILD_ANGLE "Build on ANGLE as backend for OpenGL ES" ON)
endif()

if(BUILD_GL_SOFTWARE_BCN)
  find_package(BCDec REQUIRED)
endif()

if(NOT BUILD_GLES)
  add_library(GLVersion_Dynamic INTERFACE)
  if(APPLE)
    target_compile_definitions(
      GLVersion_Dynamic INTERFACE -DGLEAM_USE_CORE -DGLEAM_MAX_VERSION=0x410
                                  -DGLEAM_MAX_VERSION_ES=0
    )
  else()
    target_compile_definitions(
      GLVersion_Dynamic INTERFACE -DGLEAM_USE_CORE -DGLEAM_MAX_VERSION=0x460
                                  -DGLEAM_MAX_VERSION_ES=0
    )
  endif()
  target_link_libraries(GLVersion_Dynamic INTERFACE GLAD)
  add_export(GLVersion_Dynamic "")
endif()
if(IOS
   OR EMSCRIPTEN
   OR BUILD_GLES
)
  add_library(GLVersion_ES INTERFACE)

  set(MAX_VERSION 0x300)
  if(BUILD_GLES_31 AND NOT (IOS OR EMSCRIPTEN))
    set(MAX_VERSION 0x310)
  endif()
  if(BUILD_GLES_32 AND NOT (IOS OR EMSCRIPTEN))
    set(MAX_VERSION 0x320)
  endif()
  if(BUILD_GLES_20)
    set(MAX_VERSION 0x200)
  endif()
  target_compile_definitions(
    GLVersion_ES
    INTERFACE -DGLEAM_USE_ES -DGLEAM_USE_LINKED -DGLEAM_MAX_VERSION=0
              -DGLEAM_MAX_VERSION_ES=${MAX_VERSION}
  )
  add_export(GLVersion_ES "")

  if(NOT IOS)
    add_library(GLVersion_ESDynamic INTERFACE)
    target_compile_definitions(
      GLVersion_ESDynamic INTERFACE -DGLEAM_USE_ES -DGLEAM_MAX_VERSION=0
                                    -DGLEAM_MAX_VERSION_ES=${MAX_VERSION}
    )
    target_link_libraries(GLVersion_ESDynamic INTERFACE GLAD)
    add_export(GLVersion_ESDynamic "")
  endif()

  add_library(GLVersion_ES2 INTERFACE)
  target_compile_definitions(
    GLVersion_ES2 INTERFACE -DGLEAM_USE_ES -DGLEAM_USE_LINKED
                            -DGLEAM_MAX_VERSION=0 -DGLEAM_MAX_VERSION_ES=0x200
  )
  add_export(GLVersion_ES2 "")
endif()

function(GEN_GLEAM_TARGET GLVERSION SUFFIX)
  coffee_library(
    TARGET GLeamCommon_${SUFFIX}
    SOURCES private/glw.cpp
            # private/levels/shared/gl_shared_debug.cpp
    LIBRARIES Core Image ComponentApp GraphicsAPI GraphicsInterface ${GLVERSION}
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
  )

  if(BUILD_GLEAM)
    coffee_library(
      TARGET GLeamRHI_${SUFFIX}
      SOURCES private/rhi/rhi.cpp private/rhi/rhi_submit.cpp
              private/rhi/rhi_texture.cpp private/rhi/rhi_translate.cpp
      LIBRARIES GLeamCommon_${SUFFIX} ComponentApp
      HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
  endif()
  if(BUILD_GL_SOFTWARE_BCN OR BUILD_GL_SOFTWARE_PVRTC)
    target_sources(
      GLeamRHI_${SUFFIX} PRIVATE private/rhi/rhi_texture_soft_decode.cpp
    )
  endif()
  if(BUILD_GL_SOFTWARE_BCN)
    target_compile_definitions(
      GLeamRHI_${SUFFIX} PUBLIC -DGLEAM_ENABLE_SOFTWARE_BCN
    )
    target_include_directories(GLeamRHI_${SUFFIX} PRIVATE ${BCDec_INCLUDE_DIR})
  endif()
  if(BUILD_GL_SOFTWARE_PVRTC)
    find_package(pvrtcdec CONFIG REQUIRED)
    target_compile_definitions(
      GLeamRHI_${SUFFIX} PUBLIC -DGLEAM_ENABLE_SOFTWARE_PVRTC
    )
    target_link_libraries(GLeamRHI_${SUFFIX} PRIVATE pvrtcdec::pvrtcdec)
  endif()

  if(${GLVERSION} MATCHES "_Dynamic")
    target_include_directories(
      GLeamCommon_${SUFFIX}
      PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gl_include>
    )
  elseif(${GLVERSION} MATCHES "_ES2?Dynamic")
    target_include_directories(
      GLeamCommon_${SUFFIX}
      PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gles_include>
    )
  endif()

  if(APPLE
     AND IOS
     AND ${GLVERSION} MATCHES "_ES"
  )
    target_compile_definitions(
      GLeamCommon_${SUFFIX} PUBLIC -DGLES_SILENCE_DEPRECATION=1
    )
    target_link_libraries(
      GLeamCommon_${SUFFIX} PUBLIC CEAGL "-framework OpenGLES"
    )
    target_include_directories(
      GLeamCommon_${SUFFIX}
      PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/coffee/windowing/private/graphics/ceagl/include>
    )
  elseif(BUILD_ANGLE AND ${GLVERSION} MATCHES "_ES")
    find_package(ANGLE CONFIG REQUIRED)
    target_link_libraries(GLeamCommon_${SUFFIX} PUBLIC AngleEGL AngleGLESv2)
  elseif(APPLE AND ${GLVERSION} MATCHES "_Dynamic")
    target_link_libraries(GLeamCommon_${SUFFIX} PUBLIC "-framework OpenGL")
  elseif(EMSCRIPTEN)
    target_link_options(
      GLeamCommon_${SUFFIX} PUBLIC
      -sMIN_WEBGL_VERSION=2
      -sMAX_WEBGL_VERSION=2
      -sFULL_ES3
    )
  elseif(${GLVERSION} MATCHES "_ESDynamic")
    # No-op
  elseif(${GLVERSION} MATCHES "_ES")
    if(ANDROID AND NOT BUILD_GLES_20)
      # On Android, GLES 3.x is used through a different library
      target_link_libraries(GLeamCommon_${SUFFIX} PUBLIC GLESv3)
    elseif(BUILD_GLES_20_STATIC)
      find_library(GLESV2_LIB GLESv2_static)
      target_link_libraries(GLeamCommon_${SUFFIX} PUBLIC ${GLESV2_LIB})
    else()
      find_library(GLESV2_LIB GLESv2)
      target_link_libraries(GLeamCommon_${SUFFIX} PUBLIC ${GLESV2_LIB})
    endif()
  endif()
endfunction()

foreach(VER Dynamic ES ESDynamic ES2)
  if(NOT TARGET GLVersion_${VER})
    continue()
  endif()
  gen_gleam_target(GLVersion_${VER} ${VER})
endforeach()

if(TARGET GLVersion_Dynamic)
  coffee_bundle(
    DESTINATION "" HEADER_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/gl_include/glad"
  )
endif()
if(TARGET GLVersion_ESDynamic OR TARGET GLVersion_ES2Dynamic)
  coffee_bundle(
    DESTINATION "" HEADER_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/gles_include/glad_es"
  )
endif()

add_subdirectory(defines)
