#pragma once
#include <GLFW/glfw3.h>
#include <betr/array.hpp>

class KeyReg {
public:
  static void init(GLFWwindow *window);
  static bool get(int key);

  KeyReg(KeyReg &&) = delete;
  KeyReg(const KeyReg &) = delete;
  KeyReg &operator=(KeyReg &&) = delete;
  KeyReg &operator=(const KeyReg &) = delete;

private:
  static betr::Array<bool, GLFW_KEY_LAST> keys;
  static void callback(GLFWwindow *window, int key, int scancode, int action, int mods);
};
