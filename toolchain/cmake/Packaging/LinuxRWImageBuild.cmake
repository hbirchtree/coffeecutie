if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(RWIMAGE_DEPLOY "${COFFEE_DEPLOY_DIRECTORY}/linux-rwimage")
  set(RWIMAGE_PACKAGED "${COFFEE_PACKAGE_DIRECTORY}/linux-rwimage")
  set(RWIMAGE_DESKTOP "${COFFEE_DESKTOP_DIRECTORY}/linux/rwimage")

  find_program(MKSQUASH_PROGRAM mksquashfs)

  set(RWIMAGE_EXTRA_LIBRARIES CACHE STRING "")

  find_package(zstd CONFIG)
  find_package(squashfuse CONFIG)
  if(squashfuse_FOUND AND zstd_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FUSE fuse)
    find_path(SQUASHFUSE_INCLUDE_DIR squashfuse/ll.h)

    if(FUSE_FOUND AND SQUASHFUSE_INCLUDE_DIR AND NOT TARGET RWImageRuntime)
      add_executable(RWImageRuntime EXCLUDE_FROM_ALL
        "${RWIMAGE_DESKTOP}/runtime.c"
      )
      target_include_directories(RWImageRuntime PRIVATE
        "${SQUASHFUSE_INCLUDE_DIR}"
        "${FUSE_INCLUDE_DIRS}"
      )
      target_link_libraries(RWImageRuntime PRIVATE
        squashfuse::squashfuse
        ${FUSE_LIBRARIES}
      )
      target_compile_options(RWImageRuntime PRIVATE
        -D_FILE_OFFSET_BITS=64
        ${FUSE_CFLAGS_OTHER}
        -ffunction-sections
        -fdata-sections
      )
      target_link_options(RWImageRuntime PRIVATE -Wl,--gc-sections)
    endif()
  endif()
endif()

function(RWIMAGE_PACKAGE)

  set(ONE_OPTS
      TARGET
      DOM_NAME
      TITLE
      VERSION_CODE
      COPYRIGHT
      COMPANY
      ICON
  )

  set(MULTI_OPTS RESOURCES BUNDLE_LIBRARIES)

  cmake_parse_arguments(RWI "" "${ONE_OPTS}" "${MULTI_OPTS}" ${ARGN})

  if(NOT MKSQUASH_PROGRAM)
    message(STATUS "mksquashfs not found - skipping RWImage for ${RWI_TARGET}")
    return()
  endif()
  if(NOT TARGET RWImageRuntime)
    message(STATUS "RWImageRuntime not available - skipping RWImage for ${RWI_TARGET}")
    return()
  endif()

  set(FINAL_FILE "${RWIMAGE_PACKAGED}/${RWI_TARGET}.rwi")
  set(SQUASH_FILE "${RWIMAGE_PACKAGED}/${RWI_TARGET}.squashfs")
  set(BASE_DIR "${RWIMAGE_DEPLOY}/${RWI_TARGET}")

  add_custom_target(${RWI_TARGET}.rwi DEPENDS ${RWI_TARGET})
  add_dependencies(${RWI_TARGET}.rwi RWImageRuntime)

  add_custom_command(
    TARGET ${RWI_TARGET}.rwi
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove "${FINAL_FILE}" "${SQUASH_FILE}"
    COMMAND ${CMAKE_COMMAND} -E make_directory
      "${BASE_DIR}/bin"
      "${BASE_DIR}/lib"
      "${BASE_DIR}/assets"
      "${RWIMAGE_PACKAGED}"
  )

  set(BIN_DIR "${BASE_DIR}/bin")
  set(LIB_DIR "${BASE_DIR}/lib")
  set(ASS_DIR "${BASE_DIR}/assets")

  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${BASE_DIR}")

  configure_file(
    "${RWIMAGE_DESKTOP}/launch.sh.in" "${BASE_DIR}/launch.sh" @ONLY
  )

  configure_file("${RWI_ICON}" "${BASE_DIR}/icon.svg" COPYONLY)

  configure_file(
    "${RWIMAGE_DESKTOP}/../appimage/set_icon.py" "${BASE_DIR}/set_icon.py"
    COPYONLY
  )

  foreach(RESC ${RWI_RESOURCES})
    add_custom_command(
      TARGET ${RWI_TARGET}.rwi
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESC}" "${ASS_DIR}"
    )
  endforeach()

  foreach(LIB ${RWI_BUNDLE_LIBRARIES} ${RWIMAGE_EXTRA_LIBRARIES})
    if(NOT EXISTS ${LIB} AND NOT "${LIB}" MATCHES "$\<")
      continue()
    endif()
    add_custom_command(
      TARGET ${RWI_TARGET}.rwi
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${LIB}" "${LIB_DIR}"
    )
  endforeach()

  if(TARGET CrashRecovery)
    add_custom_command(
      TARGET ${RWI_TARGET}.rwi
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:CrashRecovery>"
              "${BIN_DIR}/CrashRecovery"
    )
  endif()

  add_custom_command(
    TARGET ${RWI_TARGET}.rwi
    POST_BUILD
    # Copy the application binary
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${RWI_TARGET}>"
            "${BIN_DIR}/${RWI_TARGET}"
    # Make launch.sh executable
    COMMAND chmod +x "${BASE_DIR}/launch.sh"
    # Pack staged directory into a SquashFS image
    COMMAND "${MKSQUASH_PROGRAM}" "${BASE_DIR}" "${SQUASH_FILE}"
            -noappend -comp zstd -processors 1
    # Prepend the runtime ELF to produce the self-executing .rwi:
    # first copy the runtime (creates the output file), then append the squashfs.
    # Using dd avoids shell redirection and its quoting hazards entirely.
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:RWImageRuntime>" "${FINAL_FILE}"
    COMMAND dd "if=${SQUASH_FILE}" "of=${FINAL_FILE}" oflag=append conv=notrunc
    COMMAND chmod +x "${FINAL_FILE}"
    COMMAND ${CMAKE_COMMAND} -E remove "${SQUASH_FILE}"
  )

endfunction()
