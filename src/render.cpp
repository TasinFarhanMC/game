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

constexpr float PADDLE_D = 0.010f;
constexpr float WALL_E = 1.005f;

constexpr float BOOST_TIME = 0.1f;
constexpr float RECOIL_TIME = 0.3f;
constexpr float BOOST_DISTANCE = 7.0f;

struct Boost {
  Collider pad;
  float timer = 0.0f;
  float x = 0.0f;
  enum State { Idle, Boosting, Recoiling } state = Idle;

  void update(float delta_t) {
    if (state == Idle) return;

    timer += delta_t;

    if (state == Boosting) {
      float t = std::min(timer / BOOST_TIME, 1.0f);
      pad.pos.x = x + BOOST_DISTANCE * t;

      if (t >= 1.0f) {
        state = Recoiling;
        timer = 0.0f;
      }

    } else if (state == Recoiling) {
      float t = std::min(timer / RECOIL_TIME, 1.0f);
      pad.pos.x = x + BOOST_DISTANCE * (1.0f - t);

      if (t >= 1.0f) {
        pad.pos.x = x;
        state = Idle;
      }
    }
  }
};

void update_boost(Collider &pad, float delta_t) {
  static enum State { Idle, Boosting, Recoiling } state = Idle;
  static float timer = 0.0f;
  static float pos = pad.pos.x;

  if (state == Idle) return;

  timer += delta_t;

  if (state == Boosting) {
    float t = std::min(timer / BOOST_TIME, 1.0f);
    pad.pos.x = pos + BOOST_DISTANCE * t;

    if (t >= 1.0f) {
      state = Recoiling;
      timer = 0.0f;
    }

  } else if (state == Recoiling) {
    float t = std::min(timer / RECOIL_TIME, 1.0f);
    pad.pos.x = pos + BOOST_DISTANCE * (1.0f - t);

    if (t >= 1.0f) {
      pad.pos.x = pos;
      state = Idle;
    }
  }
}

struct Paddle {
  Collider collider;

  float boost_time_total = 0.15f;
  float recoil_time_total = 0.2f;

  float timer = 0.0f;
  float boost_distance = 10.0f;

  float original_x = 0.0f;

  enum State { Idle, Boosting, Recoiling } state = Idle;

  void start_boost(float distance, float boost_t, float recoil_t) {
    if (state == Idle) {
      original_x = collider.pos.x;
      boost_distance = distance;
      boost_time_total = boost_t;
      recoil_time_total = recoil_t;
      timer = 0.0f;
      state = Boosting;
    }
  }

  void update(float delta_t) {
    if (state == Idle) return;

    timer += delta_t;

    if (state == Boosting) {
      float t = std::min(timer / boost_time_total, 1.0f);
      collider.pos.x = original_x + boost_distance * t;

      if (t >= 1.0f) {
        state = Recoiling;
        timer = 0.0f;
      }

    } else if (state == Recoiling) {
      float t = std::min(timer / recoil_time_total, 1.0f);
      collider.pos.x = original_x + boost_distance * (1.0f - t);

      if (t >= 1.0f) {
        collider.pos.x = original_x;
        state = Idle;
      }
    }
  }
};

void update_pad(Collider &pad, float delta_t) {
  pad.pos += pad.vel * delta_t;

  if (pad.top() > SPACE_HEIGHT) { pad.top(SPACE_HEIGHT); }
  if (pad.bottom() < 0) { pad.bottom(0); }
}

float calc_pad_e(Collider &pad, Collider &ball) {
  return (std::abs(pad.pos.y + pad.scale.y / 2.0f - ball.pos.y - ball.scale.y / 2.0f) / (pad.scale.y - ball.scale.y)) * 2 * PADDLE_D + 1;
}

Vec2 get_ball_vel() {
  float r = 100.0f;
  static std::mt19937 gen(std::random_device {}());
  constexpr float deg_to_rad = std::numbers::pi_v<float> / 180.0f;

  // 50% chance to go left or right
  std::uniform_int_distribution<int> direction_dist(0, 1);
  bool right_side = direction_dist(gen);

  // Angle range: -60 to +60 degrees
  std::uniform_real_distribution<float> angle_dist(-45.0f * deg_to_rad, 45.0f * deg_to_rad);
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
  float paused = true;

  auto *quad_data = quads.map();
  auto *digit_data = digits.map();

  Collider pad0(quad_data[0]);
  Collider pad1(quad_data[1]);
  Collider ball(quad_data[2], get_ball_vel());

  Paddle pad0_anim {pad0};
  Paddle pad1_anim {pad1};

  auto reset_colliders = [&]() {
    ball.vel = get_ball_vel();
    ball.pos = Vec2(78.5f, 43.5f);
    paused = true;
  };

  while (!glfwWindowShouldClose(window)) {
    static float start_t = glfwGetTime(), end_t = 0, delta_t = 0, acc_t = 0;

    glClearColor(0.1, 0.2, 0.3, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    quads.render(pipeline);
    digits.render(pipeline);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    update_pad(pad0, delta_t);
    update_pad(pad1, delta_t);

    update_boost(pad1, delta_t);
    // pad0_anim.update(delta_t);

    if (!paused) {
      if (ball.colliding(pad0, delta_t)) {
        ball.pos += ball.vel * delta_t;

        float e = calc_pad_e(ball, pad0);

        ball.pos.x = pad0.left() + e * (ball.pos.x - pad0.left());
        ball.vel.x *= -e;
      } else if (ball.colliding(pad1, delta_t)) {
        ball.pos += ball.vel * delta_t;

        float e = calc_pad_e(ball, pad1);

        ball.pos.x = pad1.right() + e * (ball.pos.x - pad1.right());
        ball.vel.x *= -e;
      } else {
        ball.pos += ball.vel * delta_t;
      }

      if (ball.top() >= SPACE_HEIGHT) {
        ball.top(SPACE_HEIGHT - WALL_E * (ball.top() - SPACE_HEIGHT));
        ball.vel.y *= -WALL_E;
      } else if (ball.bottom() <= 0.0f) {
        ball.bottom(-WALL_E * ball.pos.y);
        ball.vel.y *= -WALL_E;
      }

      if (ball.right() >= SPACE_WIDTH) {
        digit_data[0].id++;
        reset_colliders();
      } else if (ball.left() <= 0.0f) {
        digit_data[1].id++;
        reset_colliders();
      }
    }

    if (digit_data[0].id > 9) {
      digit_data[0].id = 0;
      digit_data[1].id = 0;
      reset_colliders();
      std::println("Player 0 Won");
    }

    if (digit_data[1].id > 9) {
      digit_data[0].id = 0;
      digit_data[1].id = 0;
      reset_colliders();
      std::println("Player 1 Won");
    }

    static float distance = 7;
    static float boost_t = 0.1;
    static float recoil_t = 0.3;

    ImGui::DragFloat("Dis", &distance, 0.1);
    ImGui::DragFloat("Boost", &boost_t, 0.01);
    ImGui::DragFloat("Recoil", &recoil_t, 0.01);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();

    glfwSwapBuffers(window);

    end_t = glfwGetTime();
    delta_t = end_t - start_t;
    start_t = end_t;
    acc_t += delta_t;

    if (acc_t >= 1.0f / 30) {
      glfwPollEvents();

      if (KeyReg::get(GLFW_KEY_R)) { registry.reload(); }
      if (KeyReg::get(GLFW_KEY_ESCAPE)) { glfwSetWindowShouldClose(window, true); }
      if (KeyReg::get(GLFW_KEY_BACKSPACE)) {
        ball.vel = get_ball_vel();
        ball.pos = Vec2(78.5f, 43.5f);
        paused = true;
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

      if (KeyReg::get(GLFW_KEY_P)) { paused = !paused; }
      if (KeyReg::get(GLFW_KEY_RIGHT_SHIFT) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
        paused = false;
      } else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
        paused = true;
      }

      if (KeyReg::get(GLFW_KEY_SPACE)) { pad0_anim.start_boost(distance, boost_t, recoil_t); }

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
