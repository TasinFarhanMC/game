cmake_minimum_required(VERSION 3.5...3.25 FATAL_ERROR)
project(OPENGL C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")

add_subdirectory(src/glad)
add_subdirectory(src/glfw)
add_subdirectory(src/pong)
