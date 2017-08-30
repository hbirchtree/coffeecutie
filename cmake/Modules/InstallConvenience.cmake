macro(COFFEE_BUNDLE_INCLUDES )
    foreach( INC ${ARGN} )
        if( NOT INC )
            # If elements are empty, don't do anything
            continue()
        endif()
        if(NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten"))
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
        elseif(EMSCRIPTEN)

        endif()
    endforeach()
endmacro()

macro(COFFEE_BUNDLE_LIBRARY LIBRARY )
    if(ANDROID)
        install(
            FILES ${LIBRARY}
            DESTINATION "lib/${ANDROID_ABI}"
            )
    elseif(APPLE AND IOS)
        install (
            FILES ${LIBRARY}
            DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    elseif(EMSCRIPTEN)

    elseif((NOT APPLE OR IOS) AND NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten"))
        install (
            FILES ${LIBRARY}
            DESTINATION "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    endif()
endmacro()
