cmake_minimum_required(VERSION 3.25)
project(GLDL C CXX)

add_subdirectory(vendor)
add_subdirectory(src)

option(GLDL_BUILD_TESTS "Build GLDL tests" OFF)
if(GLDL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
