cmake_minimum_required(VERSION 3.11)
project(GAME C CXX)

# Make Cmake Use New OpenGL
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

include(FetchContent)

option(GLM_ENABLE_CXX_20 ON)
option(GLM_ENABLE_SIMD_SSE4_2 ON)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_SHALLOW TRUE
  GIT_TAG 1.0.1 
)
FetchContent_MakeAvailable(glm)


FetchContent_Declare(
  betr
  GIT_REPOSITORY https://github.com/TasinFarhanMC/betr.git
  GIT_SHALLOW TRUE
  GIT_TAG v0.2.4
)
FetchContent_MakeAvailable(betr)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIRS "${SRC_DIR}")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

add_executable(game "${SRC_FILES}")
target_include_directories(game PRIVATE ${INCLUDE_DIRS})
target_link_libraries(game PRIVATE OpenGL::GL X11::X11 glm::glm betr)
