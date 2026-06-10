foreach(GLIMPL Dynamic ESDynamic ES2Dynamic ES ES2)
  if(NOT TARGET GLeamCommon_${GLIMPL})
    continue()
  endif()
  coffee_library(
    TARGET GLGPUStats_${GLIMPL}
    SOURCES private/glgpustats.cpp
    LIBRARIES ComponentApp GLeamCommon_${GLIMPL}
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
  )

  if("${GLIMPL}" MATCHES "ES")
    target_compile_definitions(
      GLGPUStats_${GLIMPL} PRIVATE -DGLADCOMP_COMPILE_ES
    )
  else()
    target_compile_definitions(
      GLGPUStats_${GLIMPL} PRIVATE -DGLADCOMP_COMPILE_CORE
    )
  endif()
endforeach()
