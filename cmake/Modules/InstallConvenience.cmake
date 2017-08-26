macro(COFFEE_BUNDLE_INCLUDES INCLUDE_DIR)
    if(NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten"))
        install(
            DIRECTORY ${INCLUDE_DIR}
            DESTINATION include
            )
    elseif(EMSCRIPTEN)

    endif()
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
