set ( ELF2DOL "" CACHE STRING "" )

macro ( GAMECUBE_PACKAGE )
    cmake_parse_arguments(
        GC
        ""
        "TARGET"
        "SOURCES"
        ${ARGN}
        )

    add_executable ( "${GC_TARGET}" ${GC_SOURCES} )
    add_custom_command (
        TARGET "${GC_TARGET}"
        POST_BUILD
        COMMAND "${ELF2DOL}"
            "$<TARGET_FILE:${GC_TARGET}>"
            "$<TARGET_FILE:${GC_TARGET}>.dol"
        )
    target_link_libraries ( ${GC_TARGET} PUBLIC
        )
    install (
        FILES
        "$<TARGET_FILE:${GC_TARGET}>"
        "$<TARGET_FILE:${GC_TARGET}>.dol"
        DESTINATION
        "bin/${CMAKE_LIBRARY_ARCHITECTURE}"
        )
endmacro()
