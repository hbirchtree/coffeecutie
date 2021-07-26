foreach(GLIMPL Dynamic ESDynamic ES2Dynamic)
    coffee_library (
        TARGET GLADComponent_${GLIMPL}
        SOURCES private/glad_comp.cpp
        LIBRARIES ComponentApp
        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

    if("${GLIMPL}" MATCHES "ES")
        target_link_libraries ( GLADComponent_${GLIMPL} PUBLIC
            GLAD_ES
            )
        target_compile_definitions ( GLADComponent_${GLIMPL} PRIVATE
            -DGLADCOMP_COMPILE_ES
            )
    else()
        target_link_libraries ( GLADComponent_${GLIMPL} PUBLIC
            GLAD_Core
            )
        target_compile_definitions ( GLADComponent_${GLIMPL} PRIVATE
            -DGLADCOMP_COMPILE_CORE
            )
    endif()
endforeach()
