set(ELF2DOL
    ""
    CACHE STRING ""
)

# Freestanding apploader used to make GAMECUBE_PACKAGE's .gcm output a real, directly
# bootable disc (see toolchain/desktop/gc_apploader/apploader.c for why this is needed:
# Dolphin -- and real hardware -- only run the DOL/FST off a plain .gcm via a real
# apploader; the DirectoryBlob sys/+files/ output below skips that by booting the DOL as
# an executable instead). Built once and shared by every GAMECUBE_PACKAGE target.
# GameCube-only: Wii discs shift every disc offset right by 2 bits, which this doesn't
if(GAMECUBE AND NOT WII AND NOT TARGET gc_apploader)
  add_executable(gc_apploader
    "${CMAKE_SOURCE_DIR}/toolchain/desktop/gc_apploader/apploader.c"
  )
  target_compile_options(gc_apploader PRIVATE -ffreestanding -fno-builtin -Os)
  target_link_options(gc_apploader PRIVATE
    -nostdlib -nostartfiles -static
    -Wl,-T,"${CMAKE_SOURCE_DIR}/toolchain/desktop/gc_apploader/link.ld"
  )
  # -nostdlib drops gcc's usual implicit libgcc link too; the PPC EABI codegen for this
  # file's register-heavy prologues/epilogues (_restgpr_23_x etc.) needs it back.
  target_link_libraries(gc_apploader PRIVATE gcc)
  set_target_properties(gc_apploader PROPERTIES SUFFIX ".elf")
  add_custom_command(
    TARGET gc_apploader
    POST_BUILD
    COMMAND "${CMAKE_OBJCOPY}" -O binary
            "$<TARGET_FILE:gc_apploader>" "$<TARGET_FILE:gc_apploader>.raw.bin"
    COMMAND "${CMAKE_SOURCE_DIR}/toolchain/desktop/gc_apploader/pack_apploader.py"
            --elf "$<TARGET_FILE:gc_apploader>"
            --raw "$<TARGET_FILE:gc_apploader>.raw.bin"
            --nm "${CMAKE_NM}"
            --out "$<TARGET_FILE:gc_apploader>.img"
  )
endif()

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
  if(NOT WII)
    # gc_apploader and make_gcm.py's disc layout only implement the GameCube offset
    # convention; Wii discs shift every disc offset right by 2 bits, which neither
    # currently accounts for. Skip .gcm generation for Wii rather than emit a disc
    # image that looks right but won't boot.
    add_dependencies("${GC_TARGET}" gc_apploader)
    add_custom_command(
      TARGET "${GC_TARGET}"
      POST_BUILD
      COMMAND
        "${CMAKE_SOURCE_DIR}/toolchain/desktop/make_gcm.py"
          --dol "$<TARGET_FILE:${GC_TARGET}>.dol"
          --files "${CMAKE_CURRENT_BINARY_DIR}/${GC_TARGET}_assets"
          --apploader "$<TARGET_FILE:gc_apploader>.img"
          --out "${CMAKE_BINARY_DIR}/packaged/gamecube/${GC_TARGET}.gcm"
    )
  endif()
  install(FILES "$<TARGET_FILE:${GC_TARGET}>.elf" "$<TARGET_FILE:${GC_TARGET}>.dol"
          DESTINATION "bin/${CMAKE_LIBRARY_ARCHITECTURE}"
  )
endmacro()
