find_package ( Valgrind QUIET )

function(VALGRIND_TEST TOOL TOOL_OPTS TARGET)
    if(VALGRIND_PROGRAM)
        add_test (
            NAME "${TARGET}_${TOOL}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${VALGRIND_PROGRAM} ${TOOL_OPTS} --tool=${TOOL} $<TARGET_FILE:${TARGET}> ${ARGN}
            )
    endif()
endfunction()
