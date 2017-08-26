# CTest functions

include ( ValgrindTest )

function(COFFEE_ADD_TEST TARGET TITLE SOURCES LIBRARIES )
    # Bleh, Android kind of sucks for this.
    if(ANDROID OR WIN_UWP)
        return()
    endif()

    coffee_gen_licenseinfo("${TARGET}" "")
    coffee_gen_applicationinfo("${TARGET}"
        "${TITLE}"
        "Coffee"
        "5"
        )

    if(ANDROID) # Android platform layer
        set ( SOURCES_MOD "${SOURCES};${APPLICATION_INFO_FILE};${LICENSE_FILE};${SDL2_ANDROID_MAIN_FILE}" )
    else()
        set ( SOURCES_MOD "${SOURCES};${APPLICATION_INFO_FILE};${LICENSE_FILE}" )
    endif()

    add_executable ( ${TARGET} ${SOURCES_MOD} )

    if(EMSCRIPTEN)
        set_target_properties( ${TARGET} PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle
        )
        install(
            DIRECTORY
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle

            DESTINATION
            bin
            )
    else()
        install(
            FILES
            "$<TARGET_FILE:${TARGET}>"

            DESTINATION
            "bin/tests/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    endif()

    target_link_libraries ( ${TARGET}
        ${LIBRARIES}
        )

    target_enable_cxx11(${TARGET})

    if(ANDROID OR IOS) # Crosscompiling setup, until we find an elegant solution
        message ( "Skipping unit test: ${TITLE}" )
        message ( "Please run the tests somehow!" )
    elseif(EMSCRIPTEN)
        add_test (
            NAME ${TITLE}
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle
            COMMAND nodejs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle/${TARGET}.js
            )
    else()
        add_test (
            NAME ${TITLE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND $<TARGET_FILE:${TARGET}>
            )
    endif()

    if(COFFEE_VALGRIND_MASSIF)
        VALGRIND_TEST("massif" "--stacks=yes" "${TARGET}")
    endif()
    if(COFFEE_VALGRIND_MEMCHECK)
        VALGRIND_TEST("memcheck" "--stacks=yes" "${TARGET}")
    endif()
    if(COFFEE_VALGRIND_CALLGRIND)
        VALGRIND_TEST("callgrind" "--stacks=yes" "${TARGET}")
    endif()
    if(COFFEE_VALGRIND_CACHEGRIND)
        VALGRIND_TEST("cachegrind" "--stacks=yes" "${TARGET}")
    endif()

    if(BUILD_COVERAGE)
        setup_target_for_coverage(
            "${TARGET}.Coverage"
            "${TARGET}"
            coverage
            )
    endif()

endfunction()

