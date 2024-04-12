if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(FLATPAK_CONFIG_DIR "${COFFEE_DESKTOP_DIRECTORY}/linux/flatpak")

  set(FLATPAK_REPOSITORY_DIR CACHE PATH
                                   "Target repository to submit flatpaks to"
  )

  set(FLATPAK_PROGRAM
      "/usr/bin/flatpak"
      CACHE FILEPATH "Path to flatpak executable"
  )
  set(FLATPAK_WORKING_DIRECTORY
      "${COFFEE_DEPLOY_DIRECTORY}/linux-flatpak"
      CACHE PATH "Where to put flatpak directory structures"
  )
  set(FLATPAK_DEPLOY_DIRECTORY
      "${COFFEE_DEPLOY_DIRECTORY}/linux-flatpak"
      CACHE PATH "Where to put flatpak directory structures"
  )

  set(FLATPAK_DEFAULT_ICON_FILE
      "${COFFEE_DESKTOP_DIRECTORY}/icon.svg"
      CACHE FILEPATH "Default icon for AppImages"
  )

  set(FLATPAK_DEPLOY_LOCALLY
      FALSE
      CACHE BOOL "Deploy with local, user-only repository"
  )

  set(FLATPAK_EXTRA_LIBRARIES CACHE STRING "")
endif()

macro(
  FLATPAK_PACKAGE
  # TARGET
  # DOM_NAME
  # TITLE
  # VERSION_CODE
  # COPYRIGHT
  # COMPANY
  # DATA
  # LIBRARIES
  # BUNDLE_LIBRARIES
  # ICON_ASSET
)
  set(ONE_OPTS
      TARGET
      DOM_NAME
      TITLE
      VERSION_CODE
      COPYRIGHT
      COMPANY
      ICON_ASSET
  )
  set(MULTI_OPTS RESOURCES BUNDLE_LIBRARIES)
  cmake_parse_arguments(FLATPAK "" "${ONE_OPTS}" "${MULTI_OPTS}" ${ARGN})

  set(FLATPAK_TARGET_BRANCH "master")

  set(FLATPAK_PKG_NAME "${FLATPAK_DOM_NAME}.${FLATPAK_TARGET}")
  string(TOLOWER "${FLATPAK_PKG_NAME}" FLATPAK_PKG_NAME)

  set(FLATPAK_EXEC "${FLATPAK_TARGET}")
  set(FLATPAK_EXEC_WM "${FLATPAK_TARGET}")

  set(FLATPAK_ARCH "x86_64")
  if("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64")
    set(FLATPAK_ARCH "x86_64")
  elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "(i386|i686)-linux-gnu" OR
        "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86")
    set(FLATPAK_ARCH "i386")
  elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "(aarch64|armv8)")
    set(FLATPAK_ARCH "aarch64")
  elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm")
    set(FLATPAK_ARCH "arm")
  else()
    message(FATAL_ERROR "Flatpak architecture not defined: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()

  set(FLATPAK_BASE_DIR "${FLATPAK_DEPLOY_DIRECTORY}/${FLATPAK_PKG_NAME}")

  set(FLATPAK_ASSET_DIR "${FLATPAK_BASE_DIR}/files/assets")
  set(FLATPAK_LIBRARY_DIR "${FLATPAK_BASE_DIR}/files/lib")
  set(FLATPAK_BINARY_DIR "${FLATPAK_BASE_DIR}/files/bin")
  set(FLATPAK_EXPORT_DIR "${FLATPAK_BASE_DIR}/export")

  set(FLATPAK_ICON_REF "${FLATPAK_PKG_NAME}")

  set(FLATPAK_BUNDLE_REPO "${FLATPAK_DEPLOY_DIRECTORY}/${FLATPAK_TARGET}")
  set(FLATPAK_BUNDLE_DIR "${COFFEE_PACKAGE_DIRECTORY}/linux-flatpak/${FLATPAK_TARGET}.flatpak")

  # TODO: Unify this with the in-app information somehow
  set(FLATPAK_CONFIG "${FLATPAK_TARGET}")

  add_custom_target(${FLATPAK_TARGET}.flatpak ALL DEPENDS ${FLATPAK_TARGET})

  # Create directory structures
  add_custom_command(
    TARGET ${FLATPAK_TARGET}.flatpak
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_ASSET_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_LIBRARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_EXPORT_DIR}/share/applications"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_EXPORT_DIR}/share/icons/hicolor/scalable/apps"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${FLATPAK_DEPLOY_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${COFFEE_PACKAGE_DIRECTORY}/linux-flatpak"
  )

  # Configure metadata file
  configure_file(
    "${FLATPAK_CONFIG_DIR}/metadata.in" "${FLATPAK_BASE_DIR}/metadata" @ONLY
  )
  configure_file(
    "${FLATPAK_CONFIG_DIR}/application.desktop.in"
    "${FLATPAK_EXPORT_DIR}/share/applications/${FLATPAK_PKG_NAME}.desktop" @ONLY
  )
  configure_file(
    "${FLATPAK_ICON_ASSET}"
    "${FLATPAK_EXPORT_DIR}/share/icons/hicolor/scalable/apps/${FLATPAK_ICON_REF}.svg" COPYONLY
  )

  # Copy resources into flatpak
  foreach(RESC ${FLATPAK_RESOURCES})
    add_custom_command(
      TARGET ${FLATPAK_TARGET}.flatpak
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESC}"
              "${FLATPAK_ASSET_DIR}"
    )
  endforeach()

  # Copy bundled libraries into flatpak
  foreach(LIB ${FLATPAK_BUNDLE_LIBRARIES} ${FLATPAK_EXTRA_LIBRARIES})
    if(NOT EXISTS ${LIB})
        continue()
    endif()
    add_custom_command(
      TARGET ${FLATPAK_TARGET}.flatpak
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${LIB}" "${FLATPAK_LIBRARY_DIR}"
    )
  endforeach()

  foreach(LIB ${FLATPAK_LIBRARIES})
    add_custom_command(
      TARGET ${FLATPAK_TARGET}.flatpak
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${LIB}>"
              "${FLATPAK_LIBRARY_DIR}"
    )
  endforeach()

  # Finally, copy binary into flatpak
  add_custom_command(
    TARGET ${FLATPAK_TARGET}.flatpak
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${FLATPAK_TARGET}>"
            "${FLATPAK_BINARY_DIR}"
  )

  if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    add_custom_command(
      TARGET ${FLATPAK_TARGET}.flatpak
      POST_BUILD
      COMMAND ${CMAKE_STRIP} "${FLATPAK_BINARY_DIR}/${FLATPAK_TARGET}"
    )
  endif()

  add_custom_command(
    TARGET ${FLATPAK_TARGET}.flatpak
    POST_BUILD
    COMMAND ${FLATPAK_PROGRAM} build-export "${FLATPAK_BUNDLE_REPO}"
            "${FLATPAK_BASE_DIR}"
  )

  add_custom_command(
    TARGET ${FLATPAK_TARGET}.flatpak
    POST_BUILD
    COMMAND ${FLATPAK_PROGRAM} build-bundle --arch ${FLATPAK_ARCH} "${FLATPAK_BUNDLE_REPO}"
            "${FLATPAK_BUNDLE_DIR}" "${FLATPAK_PKG_NAME}"
  )

  if(FLATPAK_DEPLOY_LOCALLY)
    add_custom_command(
      TARGET ${FLATPAK_TARGET}.flatpak
      POST_BUILD
      COMMAND ${FLATPAK_PROGRAM} remote-add --user --no-gpg-verify
              deployed_${FLATPAK_PKG_NAME} ${FLATPAK_BUNDLE_REPO} || true
      COMMAND ${FLATPAK_PROGRAM} uninstall --user ${FLATPAK_PKG_NAME} || true
      COMMAND ${FLATPAK_PROGRAM} install --user deployed_${FLATPAK_PKG_NAME}
              ${FLATPAK_PKG_NAME}
    )
  endif()

  # Add arrangement to install flatpak structure somewhere else install (
  # DIRECTORY "${FLATPAK_BASE_DIR}"

  # DESTINATION "${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-flatpak" )
  install(FILES "${FLATPAK_BUNDLE_DIR}"
          DESTINATION "${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-flatpak"
  )
endmacro(FLATPAK_PACKAGE)
