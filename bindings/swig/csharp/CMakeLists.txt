CONFIG_SWIG_BUILD(${CFILE_SHARP})

if(COFFEE_CSHARP)
    # CSharp build
    swig_add_module(CoffeeSharp csharp ${CFILE_SHARP} )
    swig_link_libraries(CoffeeSharp ${COFFEE_LINKING} )
endif()