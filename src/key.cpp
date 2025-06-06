#include "key.hpp"
#include <GLFW/glfw3.h>
#include <betr/namespace.hpp>
#include <print>

Array<bool, GLFW_KEY_LAST> KeyReg::keys;

bool KeyReg::get(int key) {
  bool press = keys[key];
  keys[key] = false;
  return press;
}
void KeyReg::init(GLFWwindow *window) { glfwSetKeyCallback(window, callback); }

void KeyReg::callback(GLFWwindow *window, int key, int scancode, int action, int mods) { keys[key] = action == 1; }
