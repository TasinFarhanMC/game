
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <print>

#include "render.hpp"

static void glfwErrorCallback(int error, const char *desc) { std::print("GLFW Error ({}): {}", error, desc); }

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  switch (key) {
  case GLFW_KEY_ESCAPE:
    if (action == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
    break;
  }
}

static void framebuffer(GLFWwindow *window, int width, int height) {
  float min = std::min(width / SPACE_WIDTH, height / SPACE_HEIGHT);
  float m_width = min * SPACE_WIDTH;
  float m_height = min * SPACE_HEIGHT;

  glViewport(std::abs(width - m_width) / 2, std::abs(width - m_width) / 2, m_width, m_height);
}

int main() {
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    std::println("Failed to initialize GLFW");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  const GLFWvidmode *monitor_info = glfwGetVideoMode(glfwGetPrimaryMonitor());

  GLFWwindow *window = glfwCreateWindow(100, 100, "Pong", NULL, NULL);
  if (!window) {
    std::println("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwSetWindowSize(window, monitor_info->width * 2 / 3, monitor_info->height * 2 / 3);
  glfwSetWindowPos(window, monitor_info->width / 6, monitor_info->height / 6);

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer);
  glfwSetKeyCallback(window, key_callback);

  if (!gladLoadGL(glfwGetProcAddress)) {
    std::println("Unable to load GL");
    return -1;
  }

  glfwSwapInterval(1);

  int code = render(window);

  gladLoaderUnloadGL();
  glfwDestroyWindow(window);
  glfwTerminate();

  return code;
}
