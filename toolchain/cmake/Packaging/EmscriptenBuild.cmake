function(EMSCRIPTEN_PACKAGE)
  cmake_parse_arguments(
    EM "" "TARGET;SHELL;THREADS" "SOURCES;RESOURCES;ICON" ${ARGN}
  )

  set(BUNDLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EM_TARGET}.bundle)

  add_executable(${EM_TARGET} ${EM_SOURCES})
  set_target_properties(${EM_TARGET} PROPERTIES
        #
        RUNTIME_OUTPUT_DIRECTORY ${BUNDLE_DIR}
        #
        SUFFIX ".html"
  )

  set(RSC_FLAGS "")
  foreach(RSC ${EM_RESOURCES})
    set(RSC_FLAGS --preload-file=${RSC}@/assets/ ${RSC_FLAGS})
  endforeach()

  set_target_properties(
    ${EM_TARGET}
    PROPERTIES EMSCRIPTEN_THREADPOOL_SIZE 1
               EMSCRIPTEN_GROWABLE_MEMORY ON
               EMSCRIPTEN_HEAP_SIZE 512
  )
  if("${EM_SHELL}" STREQUAL "")
    set(EM_SHELL "${COFFEE_DESKTOP_DIRECTORY}/emscripten/template.html")
  endif()
  target_link_options(
    ${EM_TARGET}
    PUBLIC
    --shell-file
    "${EM_SHELL}"
    ${RSC_FLAGS}
    -sPTHREAD_POOL_SIZE=$<TARGET_PROPERTY:${EM_TARGET},EMSCRIPTEN_THREADPOOL_SIZE>
    $<$<BOOL:$<TARGET_PROPERTY:${EM_TARGET},EMSCRIPTEN_GROWABLE_MEMORY>>:-sALLOW_MEMORY_GROWTH>
    $<$<NOT:$<BOOL:$<TARGET_PROPERTY:${EM_TARGET},EMSCRIPTEN_GROWABLE_MEMORY>>>:-sTOTAL_MEMORY=$<TARGET_PROPERTY:${EM_TARGET},EMSCRIPTEN_HEAP_SIZE>MB>
  )
  add_custom_target(${EM_TARGET}WebTemplate DEPENDS ${EM_SHELL})
  add_dependencies(${EM_TARGET} ${EM_TARGET}WebTemplate)

  if("${CMAKE_BUILD_TYPE}" MATCHES ".*Deb.*")
    add_custom_command(
      TARGET ${EM_TARGET}
      COMMAND ${COFFEE_DESKTOP_DIRECTORY}/emscripten/fix-source-map.py
              ${BUNDLE_DIR}/${EM_TARGET}.wasm.map
      WORKING_DIRECTORY ${BUNDLE_DIR}
    )
  endif()

  install(DIRECTORY ${BUNDLE_DIR} DESTINATION bin)
endfunction()
