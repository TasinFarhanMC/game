cmake_minimum_required(VERSION 3.10)
project(STB)

add_library(stb STATIC "src/image.c" "include/stb/image.h")
target_include_directories(stb PUBLIC "include")
