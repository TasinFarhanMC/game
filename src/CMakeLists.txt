add_library(opengl STATIC "window.cpp")
target_include_directories(opengl PUBLIC ".")
target_link_libraries(opengl PUBLIC glfw glad glm stb_image)
