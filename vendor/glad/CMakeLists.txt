cmake_minimum_required(VERSION 3.10)
project(GLAD VERSION 2.3.3 LANGUAGES C)

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_library(glad STATIC "src/debug_gl.c" "debug_include/glad/gl.h" "debug_include/KHR/khrplatform.h")
  target_include_directories(glad PUBLIC "debug_include")
else()
  add_library(glad STATIC "src/gl.c" "include/glad/gl.h" "include/KHR/khrplatform.h")
  target_include_directories(glad PUBLIC "debug_include")
endif()

target_link_libraries(glad PRIVATE OpenGL::GL)
