CONFIG_SWIG_BUILD(${CFILE_PYTHON})

if(COFFEE_PYTHON)
    # Python build
    find_package(PythonLibs)
    include_directories ( SYSTEM
        ${PYTHON_INCLUDE_PATH}
        )

    message ( "${PYTHON_LIBRARIES}" )

    swig_add_module(CoffeePython python ${CFILE_PYTHON} )
    swig_link_libraries(CoffeePython ${COFFEE_LINKING} ${PYTHON_LIBRARIES} )

    file(GLOB PYTHON_FILES ${CMAKE_CURRENT_BINARY_DIR}/*.py)

#    message("Python files: ${PYTHON_FILES}")

    install(
        FILES
        ${PYTHON_FILES}

        DESTINATION
        share/python/
        )
endif()
