CONFIG_SWIG_BUILD(${CFILE_DLANG})

if(COFFEE_DLANG)
    # DLang build
    swig_add_module(CoffeeDLang d ${CFILE_DLANG})
    swig_link_libraries(CoffeeDLang ${COFFEE_LINKING} )

    file(GLOB DLANG_FILES ${CMAKE_CURRENT_BINARY_DIR}/*.d)

#    message("DLang files: ${DLANG_FILES}")

    install(
        FILES
        ${DLANG_FILES}

        DESTINATION
        share/dlang/
        )
endif()
