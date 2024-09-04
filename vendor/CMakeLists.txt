add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(glad)
add_subdirectory(stb)

file(GLOB IMGUI_SRC "imgui/*.cpp" "imgui/*.h" "imgui/backends/imgui_impl_glfw.cpp" "imgui/backends/imgui_impl_opengl3.cpp")

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC imgui)
target_link_libraries(imgui PUBLIC glfw)
