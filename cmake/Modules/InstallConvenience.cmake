macro(COFFEE_BUNDLE_INCLUDES )
    foreach( INC ${ARGN} )
        if( NOT INC )
            # If elements are empty, don't do anything
            continue()
        endif()
        if("${INC}" MATCHES ".*include[\/]?$")
            install(
                DIRECTORY ${INC}
                DESTINATION .
                )
        else()
            install (
                DIRECTORY ${INC}
                DESTINATION include/
                FILES_MATCHING REGEX "^.*\.(h|hpp|inl|ipp)$"
                )
        endif()
    endforeach()
endmacro()

macro(COFFEE_BUNDLE_HEADER)
    install (
        FILES ${ARGN}
        DESTINATION include/
        )
endmacro()

macro(COFFEE_BUNDLE_LIBRARY )
    foreach( LIBRARY ${ARGN} )
        if(IS_DIRECTORY "${LIBRARY}")
            install(
                DIRECTORY ${LIBRARY}
                DESTINATION lib/${CMAKE_LIBRARY_ARCHITECTURE}
                )
        elseif(ANDROID)
            install(
                FILES ${LIBRARY}
                DESTINATION "lib/${ANDROID_ABI}"
                )
        else()
            install (
                FILES ${LIBRARY}
                DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
                )
        endif()
    endforeach()
endmacro()
