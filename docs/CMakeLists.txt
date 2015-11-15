MACRO(GENERATE_DOCUMENTATION DOXYGEN_CONFIG_FILE)
FIND_PACKAGE(Doxygen)
SET(DOXYFILE_FOUND false)

set (DOXYGEN_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")

IF(EXISTS ${CMAKE_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})
        configure_file(${CMAKE_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        SET(DOXYFILE_FOUND true)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})

IF( DOXYGEN_FOUND )
        IF( DOXYFILE_FOUND )
                # Add target

                add_custom_target(make_docdir ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUT_DIR})
                ADD_CUSTOM_TARGET(doc ALL ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                        WORKING_DIRECTORY ${DOXYGEN_OUT_DIR})

        ELSE( DOXYFILE_FOUND )
                MESSAGE( STATUS "Doxygen configuration file not found - Documentation will not be generated" )
        ENDIF( DOXYFILE_FOUND )
ELSE(DOXYGEN_FOUND)
        MESSAGE(STATUS "Doxygen not found - Documentation will not be generated")
ENDIF(DOXYGEN_FOUND)
ENDMACRO(GENERATE_DOCUMENTATION)

GENERATE_DOCUMENTATION("docs/coffee.dox.in")
