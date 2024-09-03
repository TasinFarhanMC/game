cmake_minimum_required(VERSION 3.25)
project(OPENGL C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(vendor/glfw)

add_subdirectory(vendor/glm)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/stb)

add_subdirectory(src)
