# CTest functions

include ( ValgrindTest )
include ( BuildPackaging )

################################################################################
# For automating tests using CTest
################################################################################

function(COFFEE_TEST)
    # If a platform does not support simple testing, drop out here
    if(WIN_UWP)
        return()
    endif()

    cmake_parse_arguments(
        TEST
        ""
        "TARGET;TITLE"
        "SOURCES;LIBRARIES"
        ${ARGN}
        )

    if(NOT DEFINED TEST_TARGET OR NOT DEFINED TEST_TITLE)
        message ( FATAL "Test target is not valid" )
    endif()

    coffee_gen_licenseinfo(
        "${TEST_TITLE}"
        ""
        )
    coffee_gen_applicationinfo(
        "${TEST_TITLE}"
        "${TEST_TITLE}"
        "Coffee"
        "1"
        )

    if(ANDROID)
        set ( SOURCES_MOD
            ${TEST_SOURCES}
            ${APPLICATION_INFO_FILE}
            ${LICENSE_FILE}
#            ${SDL2_ANDROID_MAIN_FILE}
            )
    else()
        set ( SOURCES_MOD
            ${TEST_SOURCES}
            ${APPLICATION_INFO_FILE}
            ${LICENSE_FILE}
            )
    endif()

    if(EMSCRIPTEN)
        add_executable( ${TEST_TITLE}
            ${SOURCES_MOD}
            )

        set_target_properties( ${TEST_TITLE}
            PROPERTIES

            RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_TITLE}.bundle
            )

        install (
            DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_TITLE}.bundle
            DESTINATION bin
            )
    elseif(ANDROID OR IOS)
        coffee_application (
            TARGET "${TEST_TITLE}"
            TITLE "${TEST_TITLE}"
            COMPANY "tests"
            VERSION_CODE 1
            INFO_STRING "Unit test - ${TEST_TITLE}"
            COPYRIGHT "Testing"
            SOURCES ${SOURCES_MOD}
            LIBRARIES ${TEST_LIBRARIES} Testing
            )
    else()
        add_executable ( "${TEST_TITLE}" ${SOURCES_MOD} )

        install(
            FILES
            "$<TARGET_FILE:${TEST_TITLE}>"

            DESTINATION
            "bin/tests/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    endif()

    set ( CORE_TESTING_LIB Coffee::Testing Coffee::CoreApplication )
    if("${PROJECT_NAME}" STREQUAL "Coffee")
        set ( CORE_TESTING_LIB Testing CoreApplication )
    endif()

    if((NOT ANDROID) AND (NOT IOS))
        target_link_libraries ( ${TEST_TITLE}
            PUBLIC

            ${TEST_LIBRARIES}
            Testing
            CoreApplication
            )
    endif()

    target_enable_cxx11( ${TEST_TITLE} )

    if(IOS)
        # There is not solution for iOS as of yet

        message ( "Skipping unit test: ${TEST_TITLE}" )
        message ( "Please run the tests somehow!" )
        return()
    elseif(EMSCRIPTEN)
        add_test (
            NAME "${TEST_TITLE}"
            WORKING_DIRECTORY
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_TITLE}.bundle
            COMMAND
            nodejs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_TITLE}.bundle/${TEST_TITLE}.js
            )
    elseif(ANDROID)
        # We use a unit testing utility for Android,
        #  which installs and runs the test

        set ( ADB_AUTO_PATH
            "${CMAKE_SOURCE_DIR}/tools/automation/scripts/adb_auto" )
        set ( ADB_AUTO
            "${ADB_AUTO_PATH}/unit_test.sh" )

        set ( PKG_NAME )
        android_gen_pkg_name ("tests" "${TEST_TITLE}" PKG_NAME )

        add_test (
            NAME "${TEST_TITLE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}

            COMMAND ${ADB_AUTO}
                ${ANDROID_APK_OUTPUT_DIR}/${PKG_NAME}_dbg.apk
                ${PKG_NAME}
            )
    else()
        # In this case, CTest runs its normal course, locally
        add_test (
            NAME "${TEST_TITLE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND $<TARGET_FILE:${TEST_TITLE}>
            )
    endif()

    # Valgrind checks, for deep run-level checking
    if(VALGRIND_MASSIF)
        VALGRIND_TEST("massif" "--stacks=yes" "${TEST_TITLE}")
    endif()
    if(VALGRIND_MEMCHECK)
        VALGRIND_TEST("memcheck" "" "${TEST_TITLE}")
    endif()
    if(VALGRIND_CALLGRIND)
        VALGRIND_TEST("callgrind" "" "${TEST_TITLE}")
    endif()
    if(VALGRIND_CACHEGRIND)
        VALGRIND_TEST("cachegrind" "" "${TEST_TITLE}")
    endif()

    # When generating coverage data, we need to add the targets here
    if(BUILD_COVERAGE)
        setup_target_for_coverage(
            "${TEST_TITLE}.Coverage"
            "${TEST_TITLE}"
            coverage
            )
    endif()
endfunction()

function(COFFEE_ADD_TEST TARGET TITLE SOURCES LIBRARIES )
    coffee_test(
        TARGET "${TARGET}"
        TITLE "${TITLE}"
        SOURCES ${SOURCES}
        LIBRARIES ${LIBRARIES}
        )
endfunction()

