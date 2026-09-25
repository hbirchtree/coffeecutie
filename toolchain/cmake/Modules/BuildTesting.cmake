# ##############################################################################
# Unit tests: doctest executables, one CTest entry each
#
# Tests are not discovered per TEST_CASE: that runs the binary at build time,
# which cross builds cannot do. doctest reports each case in the output.
# ##############################################################################

function(COFFEE_ADD_TEST)
  cmake_parse_arguments(TEST "" "TARGET" "SOURCES;LIBRARIES" ${ARGN})

  if(NOT DEFINED TEST_TARGET)
    message(FATAL_ERROR "coffee_add_test: TARGET is required")
  endif()

  find_package(doctest CONFIG REQUIRED)

  add_executable(${TEST_TARGET} ${TEST_SOURCES})
  target_link_libraries(
    ${TEST_TARGET} PRIVATE ${TEST_LIBRARIES} doctest::doctest
  )
  target_compile_definitions(
    ${TEST_TARGET} PRIVATE -DCOFFEE_COMPONENT_NAME="${TEST_TARGET}"
  )

  # CMAKE_CROSSCOMPILING_EMULATOR is applied by add_test where one is set
  add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endfunction()
