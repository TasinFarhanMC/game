#include "render/quads.hpp"
#include <render/digits.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <betr/glm/vec2.hpp>
#include <betr/glm/vec3.hpp>
#include <betr/glm/vec4.hpp>
#include <betr/namespace.hpp>
#include <gl.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <shader.hpp>

using namespace gl;

constexpr float TOP_SPACE = 100.0f;
constexpr float LEFT_SPACE = 100.0f;

float from_top(float y = 0) { return TOP_SPACE - y; }
float from_left(float x = 0) { return LEFT_SPACE - x; }
float mid_y(float y = 0) { return LEFT_SPACE / 2 + y; }

int render(GLFWwindow *window) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  UniformBuffer<Vec2> simulation(0, {LEFT_SPACE, TOP_SPACE});
  UniformBuffer<Vec4> visual(1, {1.0, 0.0, 0.0, 1.0});

  ShaderRegistry registry("assets/shaders");
  Quads quads({{0.0f, mid_y(), 10, 10}, {50, mid_y(), 10, 10}}, registry);
  Digits digits({{50.0f, 50.0f, 3, 5, 1}}, registry);

  Pipeline pipeline;

  while (!glfwWindowShouldClose(window)) {
    static float start_t = glfwGetTime(), end_t = 0, delta_t = 0, acc_t = 0;

    glClear(GL_COLOR_BUFFER_BIT);

    quads.render(pipeline);
    digits.render(pipeline);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    end_t = glfwGetTime();
    delta_t = end_t - start_t;
    start_t = end_t;
    acc_t += delta_t;

    if (acc_t >= 1.0f / 30) {
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { registry.reload(); }
      glfwPollEvents();
      acc_t -= 1.0f / 30;
    }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
