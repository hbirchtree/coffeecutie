CONFIG_SWIG_BUILD(${CFILE_PYTHON})

if(COFFEE_PYTHON)
    # Python build
    find_package(PythonLibs)
    include_directories ( SYSTEM
        ${PYTHON_INCLUDE_PATH}
        )

    swig_add_module(CoffeePython python ${CFILE_PYTHON} )
    swig_link_libraries(CoffeePython ${COFFEE_LINKING} ${PYTHON_LIBRARIES} )
endif()