#include <betr/namespace.hpp>

#include "render.hpp"

#include <shaders.hpp>
#include <window.hpp>

#include <betr/def.hpp>
#include <betr/glm/vec2.hpp>

struct Vertex {
  Vec2 position;
  U8Vec2 scale;
  U8Vec2 tex;

  Vertex(float x, float y, u8 w, u8 h, u8 tex_no, u8 tex_id) : position(x, y), scale(w, h), tex(tex_no, tex_id) {}
};
void render() {
  if (!load_context()) {
    std::cerr << "Failed to load GL context" << std::endl;
    return;
  }

  if (!gladLoaderLoadGL()) {
    std::cerr << "Unable to init GLAD" << std::endl;
    return;
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  Buffer<float> base_vbo(GL_ARRAY_BUFFER, {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f});
  glBindVertexBuffer(0, base_vbo, 0, sizeof(Vec2));

  DynamicBuffer<Vertex> vbo(GL_ARRAY_BUFFER, {{160.0f - 16, 90.0f - 16, 32, 32, 0, 0}});

  glBindVertexBuffer(1, vbo, 0, sizeof(Vertex));
  glVertexBindingDivisor(1, 1);

  glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
  glEnableVertexAttribArray(0);
  glVertexAttribBinding(0, 0);

  glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribBinding(1, 1);

  glVertexAttribIFormat(2, 2, GL_UNSIGNED_BYTE, offsetof(Vertex, scale));
  glEnableVertexAttribArray(2);
  glVertexAttribBinding(2, 1);

  Program vert_program(GL_VERTEX_SHADER, vert_src);
  Program frag_program(GL_FRAGMENT_SHADER, frag_src);

  GLuint pipeline;
  glGenProgramPipelines(1, &pipeline);
  glBindProgramPipeline(pipeline);

  glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vert_program);
  glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, frag_program);

  while (running.test()) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArraysInstancedBaseInstance(GL_TRIANGLE_FAN, 0, 4, 2, 0);
    // glClearColor(0.5f, 4.0f, 3.0f, 1.0f);
    swap_buffers();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteProgramPipelines(1, &pipeline);

  unload_context();
}
