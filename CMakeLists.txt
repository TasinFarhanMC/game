cmake_minimum_required(VERSION 3.25)
project(OPENGL C CXX)

add_subdirectory(vendor)
add_subdirectory(src)

if(OPENGL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
