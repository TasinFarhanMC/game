add_library(gldl STATIC "shader.cpp")
target_include_directories(
  gldl
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC "header")
target_link_libraries(gldl PUBLIC glfw imgui glad glm stb_image)
