#include "quad.hpp"
#include <betr/namespace.hpp>

using namespace gl;

void Quads::init() {
  vao.bind();
  vao.bind_buffer(base_vertex, sizeof(Vec2));
  vao.add_attib(base_vertex, 0, 2, GL_FLOAT, false, 0);

  vao.bind_buffer(vertex, sizeof(Vertex), 1);
  vao.add_attib(vertex, 1, 2, GL_FLOAT, false, offsetof(Vertex, pos));
  vao.add_iattib(vertex, 2, 2, GL_UNSIGNED_INT, offsetof(Vertex, scale));
}

void Quads::render(Pipeline &pipeline) {
  pipeline.use_vert(registry["quad.vert"]);
  pipeline.use_frag(registry["color.frag"]);

  vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, count);
}
