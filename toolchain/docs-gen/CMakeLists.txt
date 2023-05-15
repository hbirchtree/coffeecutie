function(GENERATE_DOCUMENTATION DOXYGEN_CONFIG_FILE TARGET_SOURCE_DIRS)
  find_package(Doxygen)
  set(DOXYFILE_FOUND FALSE)

  set(DOXYGEN_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

  if(EXISTS ${DOXYGEN_CONFIG_FILE})
    configure_file(
      ${DOXYGEN_CONFIG_FILE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
    )
    set(DOXYFILE_FOUND TRUE)
  endif(EXISTS ${DOXYGEN_CONFIG_FILE})

  if(DOXYGEN_FOUND)
    if(DOXYFILE_FOUND)
      # Add target

      if(NOT BUILD_LIBRARIES)
        message("-- Building documentation automatically")
        add_custom_target(
          doc ALL
          ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
          WORKING_DIRECTORY ${DOXYGEN_OUT_DIR}
        )
      else()
        add_custom_target(
          doc ALL
          ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
          WORKING_DIRECTORY ${DOXYGEN_OUT_DIR}
        )

        set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL TRUE)
      endif()

      execute_process(
        COMMAND
          ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/stylish.css
          ${CMAKE_CURRENT_BINARY_DIR}
      )

    else(DOXYFILE_FOUND)
      message(
        STATUS
          "Doxygen configuration file not found - Documentation will not be generated"
      )
    endif(DOXYFILE_FOUND)
  else(DOXYGEN_FOUND)
    message(STATUS "Doxygen not found - Documentation will not be generated")
  endif(DOXYGEN_FOUND)
endfunction(GENERATE_DOCUMENTATION)

if(BUILD_DOCS)
  generate_documentation(
    "${CMAKE_CURRENT_SOURCE_DIR}/coffee.dox.in"
    "${CMAKE_SOURCE_DIR}/src/coffee ${CMAKE_SOURCE_DIR}/src/include"
  )
  install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html" DESTINATION "docs")
endif()
