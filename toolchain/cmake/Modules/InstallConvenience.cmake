macro(COFFEE_BUNDLE )
    cmake_parse_arguments(
        BUN
        "FORCE"
        "DESTINATION"
        "HEADER_DIRECTORIES;PUBLIC_HEADERS;LIBRARIES"
        ${ARGN}
        )

    if("${BUN_DESTINATION}" STREQUAL "")
        set ( BUN_DESTINATION "." )
    endif()

    foreach( INC ${BUN_HEADER_DIRECTORIES} )
        if( NOT INC )
            # If elements are empty, don't do anything
            continue()
        endif()
        if("${INC}" MATCHES ".*/include[\/]?$")
            install(
                DIRECTORY ${INC}
                DESTINATION ${BUN_DESTINATION}
                )
        else()
            if(NOT BUN_FORCE)
                install (
                    DIRECTORY ${INC}
                    DESTINATION ${BUN_DESTINATION}/include/
                    FILES_MATCHING REGEX "^.*\.(h|hpp|inl|ipp)$"
                    )
            else()
                install (
                    DIRECTORY ${INC}
                    DESTINATION ${BUN_DESTINATION}/include/
                    )
            endif()
        endif()
    endforeach()

    install (
        FILES ${BUN_PUBLIC_HEADERS}
        DESTINATION ${BUN_DESTINATION}/include
        )

    set ( ARCH_PREFIX "${CMAKE_LIBRARY_ARCHITECTURE}" )

    if(ANDROID)
        set ( ARCH_PREFIX "${ANDROID_ABI}" )
    endif()

    foreach( LIBRARY ${BUN_LIBRARIES} )
        if(IS_DIRECTORY "${LIBRARY}")
            install(
                DIRECTORY ${LIBRARY}
                DESTINATION "${BUN_DESTINATION}/lib/${ARCH_PREFIX}"
                )
        else()
            install (
                FILES ${LIBRARY}
                DESTINATION "${BUN_DESTINATION}/lib/${ARCH_PREFIX}"
                )
        endif()
    endforeach()
endmacro()
