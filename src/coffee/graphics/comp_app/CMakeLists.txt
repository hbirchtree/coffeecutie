option ( BUILD_GLAD_GETPROC "Force use of config-defined GetProc for GL" OFF )

foreach(GLIMPL Dynamic ESDynamic)
    coffee_library (
        TARGET GLADComponent_${GLIMPL}
        SOURCES private/glad_comp.cpp
        LIBRARIES ComponentApp GLAD
        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

    if("${GLIMPL}" MATCHES "ES")
        target_compile_definitions ( GLADComponent_${GLIMPL} PRIVATE
            -DGLADCOMP_COMPILE_ES
            )
    else()
        target_compile_definitions ( GLADComponent_${GLIMPL} PRIVATE
            -DGLADCOMP_COMPILE_CORE
            )
    endif()
    if(BUILD_GLAD_GETPROC)
        target_compile_definitions ( GLADComponent_${GLIMPL} PRIVATE
            -DGLADCOMP_USE_GETPROC
            )
    endif()
endforeach()
