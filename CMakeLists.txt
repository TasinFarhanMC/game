cmake_minimum_required(VERSION 3.11)
project(PONG C CXX)

# Make Cmake Use New OpenGL
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

include(FetchContent)

option(GLM_ENABLE_CXX_23 ON)
option(GLM_ENABLE_SIMD_SSE4_2 ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glm)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/betr)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glfw)

add_library(
  imgui STATIC
  ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp)

target_include_directories(
  imgui PUBLIC ${CMAKE_SOURCE_DIR}/vendor/imgui
               ${CMAKE_SOURCE_DIR}/vendor/imgui/backends)

target_link_libraries(imgui PUBLIC glfw)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIRS "${SRC_DIR}")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

add_executable(pong "${SRC_FILES}")
target_include_directories(pong PRIVATE ${INCLUDE_DIRS})
target_link_libraries(pong PRIVATE OpenGL::GL glm::glm glfw betr imgui)
