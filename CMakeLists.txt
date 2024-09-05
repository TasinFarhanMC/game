cmake_minimum_required(VERSION 3.25)
project(GLDL C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(vendor)
add_subdirectory(src)

option(GLDL_BUILD_TESTS "Build GLDL tests" OFF)
if(GLDL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
