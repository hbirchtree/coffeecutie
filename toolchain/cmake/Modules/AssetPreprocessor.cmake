set ( HOST_TOOLS_BINARY_DIR CACHE PATH "" )

function(COFFEE_PROCESS_ASSETS)
    cmake_parse_arguments( ASS
        "" "TARGET;OUTPUT_DIRECTORY;INPUT_DIRECTORY" "SHADER_VERSIONS"
        ${ARGN}
        )

    file ( GLOB_RECURSE ASSETS "${ASS_INPUT_DIRECTORY}/*" )

    add_custom_target( ${ASS_TARGET}
        SOURCES ${ASSETS}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASS_OUTPUT_DIRECTORY}"
        )

    foreach( FILE ${ASSETS} )
        file ( RELATIVE_PATH REL_FILE "${ASS_INPUT_DIRECTORY}" "${FILE}" )
        if(${REL_FILE} MATCHES "\.(vert|frag|comp|tess)$")
            foreach(VER ${ASS_SHADER_VERSIONS})
                string( REGEX MATCH "([a-z]+)/[1-9][0-6]0" PROFILE "${VER}")
                set ( PROFILE ${CMAKE_MATCH_1} )
                string( REGEX MATCH "[a-z]+/([1-9][0-6]0)" VERSION "${VER}")
                set ( VERSION ${CMAKE_MATCH_1} )
                get_filename_component( EXTENSION "${FILE}" EXT )
                get_filename_component( DIR "${REL_FILE}" DIRECTORY )
                get_filename_component( BASENAME "${FILE}" NAME_WE )
                string( REGEX MATCH "\.([a-z]+)$" EXTENSION "${FILE}" )
                if(${VER} STREQUAL "spv")
                    add_custom_command( TARGET ${ASS_TARGET}
                        PRE_BUILD
                        COMMAND ${CMAKE_BINARY_DIR}/bin/ShaderCooker
                            ${FILE}
                            -f
                            -B
                            -o ${ASS_OUTPUT_DIRECTORY}/${DIR}/${BASENAME}${EXTENSION}.spv
                        DEPENDS
                            ${FILE}
                        )
                    continue()
                endif()
                add_custom_command( TARGET ${ASS_TARGET}
                    PRE_BUILD
                    COMMAND ${CMAKE_BINARY_DIR}/bin/ShaderCooker
                        ${FILE}
                        -f
                        -p ${PROFILE}
                        -V ${VERSION}
                        -o ${ASS_OUTPUT_DIRECTORY}/${DIR}/${BASENAME}.${PROFILE}${VERSION}${EXTENSION}
                    DEPENDS
                        ${FILE}
                    )
            endforeach()
        else()
            add_custom_command( TARGET ${ASS_TARGET}
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E
                    copy ${FILE} ${ASS_OUTPUT_DIRECTORY}/${REL_FILE}
                DEPENDS ${FILE}
                )
        endif()
    endforeach()
endfunction()
