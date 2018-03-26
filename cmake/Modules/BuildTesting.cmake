# CTest functions

include ( ValgrindTest )
include ( BuildPackaging )

function(COFFEE_ADD_TEST TARGET TITLE SOURCES LIBRARIES )
    # Bleh, Android kind of sucks for this.
    if(WIN_UWP)
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

    if(EMSCRIPTEN)
        add_executable ( ${TARGET} ${SOURCES_MOD} )

        set_target_properties( ${TARGET} PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle
        )
        install(
            DIRECTORY
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle

            DESTINATION
            bin
            )
    elseif(ANDROID OR IOS)
        coffee_application (
            TARGET ${TARGET}
            TITLE ${TITLE}
            COMPANY "me.birchtrees"
            VERSION_CODE 1
            INFO_STRING "Unit test - ${TITLE}"
            COPYRIGHT "Testing"
            SOURCES ${SOURCES_MOD}
            LIBRARIES ${LIBRARIES} CoffeeTesting
            )
    else()
        add_executable ( ${TARGET} ${SOURCES_MOD} )

        install(
            FILES
            "$<TARGET_FILE:${TARGET}>"

            DESTINATION
            "bin/tests/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    endif()

    if(ANDROID)
    else()
        target_link_libraries ( ${TARGET}
            PUBLIC

            ${LIBRARIES}
            CoffeeTesting
            CoffeeCore_Application
            )
    endif()

    target_enable_cxx11(${TARGET})

    if(IOS) # Crosscompiling setup, until we find an elegant solution
        message ( "Skipping unit test: ${TITLE}" )
        message ( "Please run the tests somehow!" )
        return()
    elseif(EMSCRIPTEN)
        add_test (
            NAME ${TITLE}
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle
            COMMAND nodejs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bundle/${TARGET}.js
            )
    elseif(ANDROID)

        set ( ADB_AUTO_PATH
            "${CMAKE_SOURCE_DIR}/tools/automation/scripts/adb_auto" )
        set ( ADB_AUTO
            "${ADB_AUTO_PATH}/unit_test.sh" )

        set ( PKG_NAME )
        android_gen_pkg_name ("me.birchtrees" "${TARGET}" PKG_NAME )

        add_test (
            NAME ${TITLE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}

            COMMAND ${ADB_AUTO}
                ${ANDROID_APK_OUTPUT_DIR}/${PKG_NAME}_dbg.apk
                ${PKG_NAME}
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

