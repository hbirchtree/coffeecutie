CONFIG_SWIG_BUILD(${CFILE_SHARP})

if(COFFEE_CSHARP)
    # CSharp build
    swig_add_module(CoffeeSharp csharp ${CFILE_SHARP} )
    swig_link_libraries(CoffeeSharp ${COFFEE_LINKING} )

    file(GLOB CSHARP_FILES ${CMAKE_CURRENT_BINARY_DIR}/*.cs)

#    message("C# files: ${CSHARP_FILES}")

    install(
        FILES
        ${CSHARP_FILES}

        DESTINATION
        share/csharp/
        )
endif()
