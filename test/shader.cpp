#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <iostream>
#include <shader.hpp>

static void glfw_error_callback(int error, const char *description) { std::cerr << "GLFW Error " << error << ": " << description << std::endl; }

int main() {
  glfwSetErrorCallback(glfw_error_callback);

  std::cout << "Hello from test start" << std::endl;

  if (!glfwInit()) {
    std::cerr << "Unable to init glfw" << std::endl;
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  GLFWwindow *window = glfwCreateWindow(100, 100, "GLDL CLI Test Window", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Unable to create window" << std::endl;
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    std::cerr << "Unable to init glad" << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  Shader uncompiled("Uncompiled", "adsdasd", GL_VERTEX_SHADER);
  uncompiled.compile();

  Shader compiled("Compiled", "asdsad", GL_VERTEX_SHADER, true);

  glfwDestroyWindow(window);
  glfwTerminate();

  std::ofstream test_file("out.txt");
  test_file << "Test For MacOS output" << std::endl;
  test_file.close();

  return 0;
}
