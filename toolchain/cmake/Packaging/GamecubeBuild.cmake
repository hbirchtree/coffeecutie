set(ELF2DOL
    ""
    CACHE STRING ""
)

macro(GAMECUBE_PACKAGE)
    cmake_parse_arguments(GC "" "TARGET" "SOURCES;RESOURCES" ${ARGN})


  add_executable("${GC_TARGET}" ${GC_SOURCES})
  set_target_properties("${GC_TARGET}" PROPERTIES
    SUFFIX ".elf"
  )
  add_custom_command(
    TARGET "${GC_TARGET}"
    POST_BUILD
    COMMAND "${ELF2DOL}" "$<TARGET_FILE:${GC_TARGET}>"
            "$<TARGET_FILE:${GC_TARGET}>.dol"
  )
  set(_gc_res_idx 0)
  foreach(RES_DIR ${GC_RESOURCES})
    RESOURCE_DIR_PACKAGE(
      TARGET ${GC_TARGET}_resources_${_gc_res_idx}
      SOURCE_DIR ${RES_DIR}
      OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${GC_TARGET}_assets
      CACHE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${GC_TARGET}_asset_cache
    )
    add_dependencies(${GC_TARGET} ${GC_TARGET}_resources_${_gc_res_idx})
    math(EXPR _gc_res_idx "${_gc_res_idx} + 1")
  endforeach()
  add_custom_command(
    TARGET "${GC_TARGET}"
    POST_BUILD
    COMMAND
      "${CMAKE_SOURCE_DIR}/toolchain/desktop/make_gc_disc.py"
        --dol "$<TARGET_FILE:${GC_TARGET}>.dol"
        --files "${CMAKE_CURRENT_BINARY_DIR}/${GC_TARGET}_assets"
        --out "${CMAKE_BINARY_DIR}/packaged/gamecube/${GC_TARGET}"
  )
  install(FILES "$<TARGET_FILE:${GC_TARGET}>.elf" "$<TARGET_FILE:${GC_TARGET}>.dol"
          DESTINATION "bin/${CMAKE_LIBRARY_ARCHITECTURE}"
  )
endmacro()
