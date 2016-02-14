CONFIG_SWIG_BUILD(${CFILE_DLANG})

if(COFFEE_DLANG)
    # DLang build
    swig_add_module(CoffeeDLang d ${CFILE_DLANG})
    swig_link_libraries(CoffeeDLang ${COFFEE_LINKING} )
endif()