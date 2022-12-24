function ( EMSCRIPTEN_PACKAGE )
    cmake_parse_arguments (
        EM
        ""
        "TARGET;SHELL;THREADS"
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
        set ( RSC_FLAGS --preload-file=${RSC}@/assets/ ${RSC_FLAGS} )
    endforeach()

    set ( THREAD_FLAG "" )
    if(NOT "${EM_THREADS}" STREQUAL "")
        set ( THREAD_FLAG "-sPTHREAD_POOL_SIZE=${EM_THREADS}" )
    endif()
    if("${EM_SHELL}" STREQUAL "")
        set ( EM_SHELL "${COFFEE_DESKTOP_DIRECTORY}/emscripten/template.html" )
    endif()
    target_link_options ( ${EM_TARGET} PUBLIC
        --shell-file "${EM_SHELL}"
        ${RSC_FLAGS}
        ${THREAD_FLAG}
        )
    add_custom_target ( ${EM_TARGET}WebTemplate
        DEPENDS ${EM_SHELL}
        )
    add_dependencies ( ${EM_TARGET} ${EM_TARGET}WebTemplate)

    if("${CMAKE_BUILD_TYPE}" MATCHES ".*Deb.*")
        add_custom_command ( TARGET ${EM_TARGET}
            COMMAND ${COFFEE_DESKTOP_DIRECTORY}/emscripten/fix-source-map.py
            ${BUNDLE_DIR}/${EM_TARGET}.wasm.map
            WORKING_DIRECTORY ${BUNDLE_DIR}
            )
    endif()

    install( DIRECTORY ${BUNDLE_DIR} DESTINATION bin )
endfunction()
