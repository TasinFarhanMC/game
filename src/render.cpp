#define GLFW_INCLUDE_NONE

#include <betr/glm/vec2.hpp>
#include <betr/glm/vec3.hpp>
#include <betr/glm/vec4.hpp>
#include <betr/namespace.hpp>

#include <gl.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <key.hpp>
#include <render.hpp>
#include <render/digits.hpp>
#include <render/quads.hpp>
#include <shader.hpp>

float from_left(float x) { return SPACE_WIDTH - x; }

using namespace gl;

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

  UniformBuffer<Vec2> sim(0, {SPACE_WIDTH, SPACE_HEIGHT});
  UniformBuffer<Vec4> visual(1, {1.0f, 1.0f, 1.0f, 1.0f});

  ShaderReg registry("assets/shaders");
  Quads quads({{15.0f, 35.0f, 3, 20}, {142, 35, 3, 20}, {78.5f, 43.5f, 3, 3}}, registry);
  Digits digits({{4, 76, 6, 10, 0}, {150, 76, 6, 10, 1}}, registry);

  Pipeline pipeline;

  Vec2 pad0_vel;
  Vec2 pad1_vel;

  while (!glfwWindowShouldClose(window)) {
    static float start_t = glfwGetTime(), end_t = 0, delta_t = 0, acc_t = 0;

    glClear(GL_COLOR_BUFFER_BIT);

    quads.render(pipeline);
    digits.render(pipeline);

    auto *quad_data = quads.map();
    auto &pad0 = quad_data[0];
    auto &pad1 = quad_data[1];

    pad0.pos += pad0_vel * delta_t;
    pad1.pos += pad1_vel * delta_t;

    if (pad0.pos.y + pad0.scale.y > SPACE_HEIGHT) { pad0.pos.y = SPACE_HEIGHT - pad0.scale.y; }
    if (pad1.pos.y + pad1.scale.y > SPACE_HEIGHT) { pad1.pos.y = SPACE_HEIGHT - pad1.scale.y; }

    if (pad0.pos.y < 0) { pad0.pos.y = 0; }
    if (pad1.pos.y < 0) { pad1.pos.y = 0; }
    quads.unmap();

    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    // float spacing = ImGui::GetStyle().ItemSpacing.x;
    //
    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    end_t = glfwGetTime();
    delta_t = end_t - start_t;
    start_t = end_t;
    acc_t += delta_t;

    if (acc_t >= 1.0f / 30) {
      glfwPollEvents();

      if (KeyReg::get(GLFW_KEY_SPACE)) { registry.reload(); }
      if (KeyReg::get(GLFW_KEY_ESCAPE)) { glfwSetWindowShouldClose(window, true); }

      if (!(glfwGetKey(window, GLFW_KEY_W) ^ glfwGetKey(window, GLFW_KEY_S))) {
        pad0_vel.y = 0.0f;
      } else if (glfwGetKey(window, GLFW_KEY_W)) {
        pad0_vel.y = SPACE_HEIGHT;
      } else if (glfwGetKey(window, GLFW_KEY_S)) {
        pad0_vel.y = -SPACE_HEIGHT;
      }

      if (!(glfwGetKey(window, GLFW_KEY_UP) ^ glfwGetKey(window, GLFW_KEY_DOWN))) {
        pad1_vel.y = 0.0f;
      } else if (glfwGetKey(window, GLFW_KEY_UP)) {
        pad1_vel.y = SPACE_HEIGHT;
      } else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        pad1_vel.y = -SPACE_HEIGHT;
      }

      acc_t -= 1.0f / 30;
    }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
