FUNCTION(GENERATE_DOCUMENTATION DOXYGEN_CONFIG_FILE TARGET_SOURCE_DIRS)
    FIND_PACKAGE(Doxygen)
    SET ( DOXYFILE_FOUND FALSE )

    set (DOXYGEN_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    IF(EXISTS ${DOXYGEN_CONFIG_FILE})
        CONFIGURE_FILE(
            ${DOXYGEN_CONFIG_FILE}
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        SET ( DOXYFILE_FOUND TRUE )
    ENDIF(EXISTS ${DOXYGEN_CONFIG_FILE})

    IF( DOXYGEN_FOUND )
        IF( DOXYFILE_FOUND )
            # Add target

            if(NOT COFFEE_GENERATE_LIBRARIES)
                MESSAGE ( "-- Building documentation automatically" )
                ADD_CUSTOM_TARGET( doc ALL
                    ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                    WORKING_DIRECTORY ${DOXYGEN_OUT_DIR})
            else()
                ADD_CUSTOM_TARGET( doc ALL
                    ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                    WORKING_DIRECTORY ${DOXYGEN_OUT_DIR})

                set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL TRUE)
            endif()
            
            execute_process (
                COMMAND ${CMAKE_COMMAND} -E copy 
                    ${CMAKE_CURRENT_SOURCE_DIR}/stylish.css
                    
                    ${CMAKE_CURRENT_BINARY_DIR}
                )

        ELSE( DOXYFILE_FOUND )
            MESSAGE( STATUS "Doxygen configuration file not found - Documentation will not be generated" )
        ENDIF( DOXYFILE_FOUND )
    ELSE(DOXYGEN_FOUND)
        MESSAGE(STATUS "Doxygen not found - Documentation will not be generated")
    ENDIF(DOXYGEN_FOUND)
ENDFUNCTION(GENERATE_DOCUMENTATION)

IF(COFFEE_DOCS)
    GENERATE_DOCUMENTATION(
        "${CMAKE_SOURCE_DIR}/docs-gen/coffee.dox.in"
        "${CMAKE_SOURCE_DIR}/src/coffee"
        )
    INSTALL (
        DIRECTORY
        "${CMAKE_BINARY_DIR}/docs-gen/html"
        DESTINATION
        "docs"
        )
ENDIF()
