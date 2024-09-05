#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <shader.hpp>

int main() {
  if (!glfwInit()) return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  GLFWwindow *window = glfwCreateWindow(1, 1, "GLDL CLI Test Window", NULL, NULL);
  if (window == NULL) return -1;

  glfwMakeContextCurrent(window);
  gladLoadGL((GLADloadfunc)glfwGetProcAddress);

  Shader uncompiled("Uncompiled", " ", GL_VERTEX_SHADER);
  uncompiled.compile();

  Shader compiled("Compiled", " ", GL_VERTEX_SHADER, true);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
