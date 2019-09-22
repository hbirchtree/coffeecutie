macro ( EMSCRIPTEN_PACKAGE )
    cmake_parse_arguments (
        EM
        ""
        "TARGET"
        "SOURCES;RESOURCES;ICON"
        ${ARGN}
        )

    add_executable(${EM_TARGET} ${EM_SOURCES})
    set_property ( TARGET ${EM_TARGET}
        PROPERTY SUFFIX ".html" )
    set_target_properties(
        ${EM_TARGET}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EM_TARGET}.bundle
        )
    if(NOT DEFINED "${EM_TARGET}_RSC_FLAGS")
        set ( "${EM_TARGET}_RSC_FLAGS" "" )

        foreach (RSC ${EM_RESOURCES})
            set (
                "${EM_TARGET}_RSC_FLAGS"
                "--preload-file ${RSC}@/assets/ ${${EM_TARGET}_RSC_FLAGS}"
                )
        endforeach()
    endif()

    set_target_properties ( ${EM_TARGET}
        PROPERTIES
        LINK_FLAGS "${${EM_TARGET}_RSC_FLAGS}"
        )

    if("${CMAKE_BUILD_TYPE}" MATCHES ".*Deb.*")
        add_custom_command ( TARGET ${EM_TARGET}
            COMMAND ${COFFEE_DESKTOP_DIRECTORY}/emscripten/fix-source-map.py
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EM_TARGET}.bundle/${EM_TARGET}.wasm.map
            )
    endif()

    install(
        DIRECTORY
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EM_TARGET}.bundle

        DESTINATION
        bin
        )
endmacro()
