function ( EMSCRIPTEN_PACKAGE )
    cmake_parse_arguments (
        EM
        ""
        "TARGET;SHELL"
        "SOURCES;RESOURCES;ICON"
        ${ARGN}
        )

    set ( BUNDLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EM_TARGET}.bundle )

    add_executable(${EM_TARGET} ${EM_SOURCES})
    set_target_properties ( ${EM_TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${BUNDLE_DIR}
        SUFFIX ".html"
        )

    set ( RSC_FLAGS "" )
    foreach (RSC ${EM_RESOURCES})
        set ( RSC_FLAGS "--preload-file ${RSC}@/assets/ ${RSC_FLAGS}" )
    endforeach()

    if("${EM_SHELL}" STREQUAL "")
        set ( EM_SHELL "${COFFEE_DESKTOP_DIRECTORY}/emscripten/template.html" )
    endif()

    target_link_options ( ${EM_TARGET} PUBLIC
        ${RSC_FLAGS}
        --shell-file "${EM_SHELL}"
        )

    if("${CMAKE_BUILD_TYPE}" MATCHES ".*Deb.*")
        add_custom_command ( TARGET ${EM_TARGET}
            COMMAND ${COFFEE_DESKTOP_DIRECTORY}/emscripten/fix-source-map.py
            ${BUNDLE_DIR}/${EM_TARGET}.wasm.map
            WORKING_DIRECTORY ${BUNDLE_DIR}
            )
    endif()

    install( DIRECTORY ${BUNDLE_DIR} DESTINATION bin )
endfunction()
