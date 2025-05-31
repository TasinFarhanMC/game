#include "render.hpp"
#include "shaders.hpp"
#include <GL/gl.h>
#include <betr/glm/vec2.hpp>
#include <betr/glm/vec4.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace betr;

#include <glad/gl.h>
#include <print>

// Callback for error reporting
void glfwErrorCallback(int error, const char *desc) {
  std::print("GLFW Error ({}): {}", error, desc);
}

void framebuffer(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
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

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "GLFW Starter Template", NULL, NULL);
  if (!window) {
    std::println("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer);

  if (!gladLoadGL(glfwGetProcAddress)) {
    std::println("Unable to load GL");
    return -1;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  glfwSwapInterval(1);

  VertexArray vertex_array;
  Buffer<Vec2> vertex(GL_ARRAY_BUFFER,
                      {{50, 50}, {-50, 50}, {-50, -50}, {-50, 50}},
                      GL_DYNAMIC_DRAW);

  vertex_array.bind_buffer(vertex, sizeof(Vec2));
  vertex_array.add_attib(vertex, 0, 2, GL_FLOAT, false, 0);

  Program vert_program(GL_VERTEX_SHADER, vert_src);
  Program frag_program(GL_FRAGMENT_SHADER, frag_src);

  UniformBuffer<Vec4> ubo(0, {1.0, 0.0, 0.0, 0.0});

  Pipeline pipeline;
  pipeline.use_stages(GL_VERTEX_SHADER_BIT, vert_program);
  pipeline.use_stages(GL_FRAGMENT_SHADER_BIT, frag_program);

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Color Picker");
    Vec4 color = ubo.get();
    if (ImGui::ColorPicker4("Color", &color.x)) {
      ubo.set(color);
      ubo.upload();
    }
    ImGui::End();

    glDrawArrays(GL_TRIANGLES, 0, 3);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
