#include "render/digits.hpp"
#define GLFW_INCLUDE_NONE

#include "render/quads.hpp"
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

#include "render.hpp"

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

  ShaderRegistry registry("assets/shaders");
  Quads quads({{15.0f, 35.0f, 3, 20}, {}, {0, 0, 3, 3}}, registry);
  Digits digits({{4, 76, 6, 10, 1}, {0, 0, 6, 10, 1}}, registry);

  Pipeline pipeline;

  while (!glfwWindowShouldClose(window)) {
    static float start_t = glfwGetTime(), end_t = 0, delta_t = 0, acc_t = 0;

    glClear(GL_COLOR_BUFFER_BIT);

    quads.render(pipeline);
    digits.render(pipeline);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    {
      ImGui::Begin("Editor");
      auto *q_data = quads.map();
      auto *d_data = digits.map();

      {
        ImGui::BeginChild("Paddle", ImVec2(), ImGuiChildFlags_AutoResizeY);
        ImGui::Text("Paddle:");

        ImGui::DragFloat2("Pos", glm::value_ptr(q_data[0].pos), 0.5f);
        ImGui::DragInt2("Scale", (int *)glm::value_ptr(q_data[0].scale));

        q_data[1].scale = q_data[0].scale;
        q_data[1].pos = Vec2(from_left(q_data[0].pos.x) - q_data[0].scale.x, q_data[0].pos.y);

        ImGui::EndChild();
      }

      {
        ImGui::BeginChild("Ball", ImVec2(), ImGuiChildFlags_AutoResizeY);
        ImGui::Text("Ball:");

        unsigned &scale = q_data[2].scale.x;
        ImGui::DragInt("Scale", (int *)&scale);
        q_data[2].scale = UVec2(scale);
        q_data[2].pos = Vec2((SPACE_WIDTH - scale) / 2, (SPACE_HEIGHT - scale) / 2);

        ImGui::EndChild();
      }

      {
        ImGui::BeginChild("Digit", ImVec2(), ImGuiChildFlags_AutoResizeY);
        ImGui::Text("Digit:");

        ImGui::DragFloat2("Pos", glm::value_ptr(d_data[0].pos), 0.5f);
        ImGui::DragInt("Scale", (int *)&d_data[0].scale.x);
        d_data[0].scale.y = d_data[0].scale.x * 5 / 3;

        int id = d_data[0].id;
        ImGui::DragInt("Id", &id);
        d_data[0].id = id;

        d_data[1].scale = d_data[0].scale;
        d_data[1].id = d_data[0].id;
        d_data[1].pos = Vec2(from_left(d_data[0].pos.x) - d_data[0].scale.x, d_data[0].pos.y);

        ImGui::EndChild();
      }

      quads.unmap();
      digits.unmap();
      ImGui::End();
    }

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
