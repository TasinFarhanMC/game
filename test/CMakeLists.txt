message(STATUS "Building tests")

file(GLOB_RECURSE TESTS "*.cpp")

foreach(TEST_FILE ${TESTS})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  string(CONCAT TEST_NAME "gldl_" ${TEST_NAME})
  string(TOUPPER "${TEST_NAME}" CTEST_NAME)

  message(STATUS "Test Name: ${TEST_NAME}, CTest Name: ${CTEST_NAME}, Label: ${TEST_LABEL}")

  add_executable(${TEST_NAME} ${TEST_FILE})
  target_link_libraries(${TEST_NAME} PRIVATE gldl)
  add_test(NAME ${CTEST_NAME} COMMAND ${TEST_NAME})

  if(${TEST_NAME} MATCHES "^gldl_gui_")
    set(TEST_LABEL "GUI")
  else()
    set(TEST_LABEL "CLI")
  endif()

  set_tests_properties(${CTEST_NAME} PROPERTIES LABELS ${TEST_LABEL})
endforeach()
