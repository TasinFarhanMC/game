cmake_minimum_required(VERSION 3.10)
project(STB)

add_library(stb_image STATIC "src/image.c" "include/image/stb/image.h")
target_include_directories(stb_image PUBLIC "include/image")
