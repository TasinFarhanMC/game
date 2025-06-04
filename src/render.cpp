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

struct QuadVertex {
  Vec2 pos;
  UVec2 scale;

  QuadVertex(float x, float y, unsigned w, unsigned h) : pos(x, y), scale(w, h) {}
};

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

  VertexArray vertex_array;
  Buffer<Vec2> quad_vert(
      GL_ARRAY_BUFFER, {{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}},
      GL_STATIC_DRAW
  );

  Buffer<QuadVertex> vertex(GL_ARRAY_BUFFER, {{0.0f, 0.0f, 10, 10}}, GL_DYNAMIC_DRAW);

  vertex_array.bind_buffer(quad_vert, sizeof(Vec2));
  vertex_array.add_attib(quad_vert, 0, 2, GL_FLOAT, false, 0);

  vertex_array.bind_buffer(vertex, sizeof(QuadVertex), 6);
  vertex_array.add_attib(vertex, 1, 2, GL_FLOAT, false, offsetof(QuadVertex, pos));
  vertex_array.add_iattib(vertex, 2, 2, GL_UNSIGNED_INT, offsetof(QuadVertex, scale));

  UniformBuffer<Vec4> ubo(0, {1.0, 0.0, 0.0, 1.0});

  ShaderRegistry registry("shaders");

  Pipeline pipeline;

  Vec3 bg_color(0.1f, 0.2f, 0.3f);

  Vec2 vel;
  float e = 1.0f;

  while (!glfwWindowShouldClose(window)) {
    pipeline.use_vert(registry["quad.vert"]);
    pipeline.use_frag(registry["color.frag"]);

    static float start_t = glfwGetTime(), end_t = 0, delta_t = 0, acc_t = 0;

    glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    {
      ImGui::Begin("Visuals");
      float pickerWidth = (ImGui::GetContentRegionAvail().x - spacing) * 0.5f;
      float w = pickerWidth + spacing;

      ImGui::Text("Color");
      ImGui::SameLine(w, spacing);
      ImGui::Text("Background Color");

      Vec4 *color = ubo.map();
      ImGui::SetNextItemWidth(pickerWidth);
      ImGui::ColorPicker4(
          "##color", (float *)color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar
      );
      ubo.unmap();

      ImGui::SameLine();
      ImGui::SetNextItemWidth(pickerWidth);
      ImGui::ColorPicker3("##bg_color", (float *)&bg_color, ImGuiColorEditFlags_NoSidePreview);
      ImGui::End();
    }

    {
      static bool running = false;
      ImGui::Begin("Simulation");
      QuadVertex *data = vertex.map();

      if (!running) {
        running = ImGui::Button("Start");
      } else {
        running = !ImGui::Button("Stop");
      }

      ImGui::SameLine();
      if (ImGui::Button("Reset")) {
        data->pos = Vec2(0);
        vel = Vec2(0);
        data->scale = UVec2(10);
        e = 1.0f;
        running = false;
      }

      ImGui::SliderFloat2("Pos", glm::value_ptr(data->pos), 0.0f, 100.0f);
      ImGui::SliderFloat2("vel", glm::value_ptr(vel), -0.1f, 0.1f);
      ImGui::SliderInt2("Scale", glm::value_ptr(data->scale), 1, 100);
      ImGui::SliderFloat("Bounce", &e, 0.0f, 2.0f);

      if (running) {
        data->pos += vel * delta_t;

        if ((data->pos.y + data->scale.y) >= 100.0f) {
          data->pos.y = 100.0f - e * (data->pos.y + data->scale.y - 100.0f) - data->scale.y;
          vel.y *= -e;
        } else if (data->pos.y <= 0.0f) {
          data->pos.y = -e * data->pos.y;
          vel.y *= -e;
        }

        if ((data->pos.x + data->scale.x) >= 100.0f) {
          data->pos.x = 100.0f - e * (data->pos.x + data->scale.x - 100.0f) - data->scale.x;
          vel.x *= -e;
        } else if (data->pos.x <= 0.0f) {
          data->pos.x = -e * data->pos.x;
          vel.x *= -e;
        }
      }

      vertex.unmap();

      ImGui::End();
    }

  GUI_END:
    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    end_t = glfwGetTime();
    delta_t = end_t - start_t;
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
