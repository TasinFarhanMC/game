#include <numbers>
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
#include <physics.hpp>
#include <random>
#include <render.hpp>
#include <render/digits.hpp>
#include <render/quads.hpp>
#include <shader.hpp>

float from_left(float x) { return SPACE_WIDTH - x; }

using namespace gl;

void update_pad(Collider &pad, float delta_t) {
  pad.pos += pad.vel * delta_t;

  if (pad.top() > SPACE_HEIGHT) { pad.top(SPACE_HEIGHT); }
  if (pad.bottom() < 0) { pad.bottom(0); }
}

void ball_pad_collision(Collider &ball, Collider &pad, float delta_t, float &pad_bon) {
  ball.pos += ball.vel * delta_t;
  float e =
      (std::abs(pad.pos.y + pad.scale.y / 2.0f - ball.pos.y - ball.scale.y / 2.0f) / (pad.scale.y - ball.scale.y)) /
          0.5 * pad_bon +
      1;

  ball.pos.x = pad.pos.x + e * (ball.pos.x - pad.pos.x);
  ball.vel.x *= -e;
}

Vec2 ball_vel() {
  float r = 100.0f;
  static std::mt19937 gen(std::random_device {}());
  constexpr float deg_to_rad = std::numbers::pi_v<float> / 180.0f;

  // 50% chance to go left or right
  std::uniform_int_distribution<int> direction_dist(0, 1);
  bool right_side = direction_dist(gen);

  // Angle range: -60 to +60 degrees
  std::uniform_real_distribution<float> angle_dist(-60.0f * deg_to_rad, 60.0f * deg_to_rad);
  float angle = angle_dist(gen);

  // Flip across the y-axis if going left
  if (!right_side) { angle = std::numbers::pi_v<float> - angle; }

  // 50% chance to flip the y component (vertical direction)
  std::uniform_int_distribution<int> flip_y_dist(0, 1);
  bool flip_y = flip_y_dist(gen);

  float x = std::cos(angle);
  float y = std::sin(angle) * (flip_y ? -1.0f : 1.0f);

  return Vec2(x, y) * r;
}

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
  Digits digits({{4, 76, 6, 10, 0}, {150, 76, 6, 10, 0}}, registry);

  Pipeline pipeline;

  float wall_bon = 0.005f;
  float pad_bon = 0.010f;
  float run = false;
  auto *quad_data = quads.map();
  auto *digit_data = digits.map();

  Collider pad0(quad_data[0]);
  Collider pad1(quad_data[1]);
  Collider ball(quad_data[2], ball_vel());

  while (!glfwWindowShouldClose(window)) {
    static float start_t = glfwGetTime(), end_t = 0, delta_t = 0, acc_t = 0;

    glClearColor(0.1, 0.2, 0.3, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    quads.render(pipeline);
    digits.render(pipeline);

    if (KeyReg::get(GLFW_KEY_ENTER)) { run = !run; }
    if (KeyReg::get(GLFW_KEY_RIGHT_SHIFT) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
      run = true;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
      run = false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    update_pad(pad0, delta_t);
    update_pad(pad1, delta_t);

    if (run) {
      if (ball.colliding(pad0, delta_t)) {
        ball_pad_collision(ball, pad0, delta_t, pad_bon);
      } else if (ball.colliding(pad1, delta_t)) {
        ball_pad_collision(ball, pad1, delta_t, pad_bon);
      } else {
        ball.pos += ball.vel * delta_t;
      }

      if ((ball.pos.y + ball.scale.y) >= SPACE_HEIGHT) {
        ball.pos.y = SPACE_HEIGHT - (1 + wall_bon) * (ball.pos.y + ball.scale.y - SPACE_HEIGHT) - ball.scale.y;
        ball.vel.y *= -(1 + wall_bon);
      } else if (ball.pos.y <= 0.0f) {
        ball.pos.y = -(1 + wall_bon) * ball.pos.y;
        ball.vel.y *= -(1 + wall_bon);
      }

      // if ((ball.pos.x + ball.scale.x) >= SPACE_WIDTH) {
      //   ball.pos.x = SPACE_WIDTH - (1 + wall_bon) * (ball.pos.x + ball.scale.x - SPACE_WIDTH) - ball.scale.x;
      //   ball.vel.x *= -(1 + wall_bon);
      // } else if (ball.pos.x <= 0.0f) {
      //   ball.pos.x = -(1 + wall_bon) * ball.pos.x;
      //   ball.vel.x *= -(1 + wall_bon);
      // }

      if ((ball.pos.x + ball.scale.x) >= SPACE_WIDTH) {
        digit_data[0].id++;
        ball.vel = ball_vel();
        ball.pos = Vec2(78.5f, 43.5f);
        run = false;
      } else if (ball.pos.x <= 0.0f) {
        digit_data[1].id++;
        ball.vel = ball_vel();
        ball.pos = Vec2(78.5f, 43.5f);
        run = false;
      }

      // run = false;
    }

    if (digit_data[0].id > 9) {
      ball.vel = ball_vel();
      ball.pos = Vec2(78.5f, 43.5f);
      digit_data[0].id = 0;
      digit_data[1].id = 0;
      run = false;
      std::println("Player 0 Won");
    }

    if (digit_data[1].id > 9) {
      ball.vel = ball_vel();
      ball.pos = Vec2(78.5f, 43.5f);
      digit_data[0].id = 0;
      digit_data[1].id = 0;
      run = false;
      std::println("Player 1 Won");
    }

    ImGui::DragFloat("Wall Bounce", &wall_bon, 0.005f);
    ImGui::DragFloat("Pad Bounce", &pad_bon, 0.005f);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    end_t = glfwGetTime();
    delta_t = end_t - start_t;
    start_t = end_t;
    acc_t += delta_t;

    if (acc_t >= 1.0f / 30) {
      glfwPollEvents();

      if (KeyReg::get(GLFW_KEY_SPACE)) { registry.reload(); }
      if (KeyReg::get(GLFW_KEY_ESCAPE)) { glfwSetWindowShouldClose(window, true); }
      if (KeyReg::get(GLFW_KEY_BACKSPACE)) {
        ball.vel = ball_vel();
        ball.pos = Vec2(78.5f, 43.5f);
        run = false;
      }

      if (!(glfwGetKey(window, GLFW_KEY_W) ^ glfwGetKey(window, GLFW_KEY_S))) {
        pad0.vel.y = 0.0f;
      } else if (glfwGetKey(window, GLFW_KEY_W)) {
        pad0.vel.y = SPACE_HEIGHT;
      } else if (glfwGetKey(window, GLFW_KEY_S)) {
        pad0.vel.y = -SPACE_HEIGHT;
      }

      if (!(glfwGetKey(window, GLFW_KEY_UP) ^ glfwGetKey(window, GLFW_KEY_DOWN))) {
        pad1.vel.y = 0.0f;
      } else if (glfwGetKey(window, GLFW_KEY_UP)) {
        pad1.vel.y = SPACE_HEIGHT;
      } else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        pad1.vel.y = -SPACE_HEIGHT;
      }

      acc_t -= 1.0f / 30;
    }
  }

  quads.unmap();
  digits.unmap();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
