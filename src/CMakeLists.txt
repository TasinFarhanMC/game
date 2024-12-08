add_library(gldl STATIC "shader.cpp")
target_include_directories(
  gldl
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC "headers")

if(GLDL_BUILD_REDIST)
  target_link_libraries(gldl PRIVATE glfw imgui glad glm stb)
else()
  target_link_libraries(gldl PUBLIC glfw imgui glad glm stb)
endif()
