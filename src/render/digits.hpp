#pragma once
#include "shader.hpp"
#include <betr/glm/ext/vec_float2.hpp>
#include <betr/glm/ext/vec_uint2.hpp>
#include <gl.hpp>

class Digits {
public:
  struct Vertex {
    betr::Vec2 pos;
    betr::UVec2 scale;
    unsigned char id;

    Vertex(float x, float y, unsigned w, unsigned h, unsigned char id) : pos(x, y), scale(w, h), id(id) {}
  };

  Digits(betr::InitList<Vertex> data, ShaderRegistry &registry)
      : vertex(GL_ARRAY_BUFFER, data),
        base_vertex(GL_ARRAY_BUFFER, {{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}}),
        registry(registry) {
    count = data.size();
    init();
  };

  void render(gl::Pipeline &pipeline);

  Vertex *map() { return vertex.map(); }
  void unmap() { vertex.unmap(); }

  Digits(Digits &&) = delete;
  Digits(const Digits &) = delete;
  Digits &operator=(Digits &&) = delete;
  Digits &operator=(const Digits &) = delete;

private:
  gl::Buffer<Vertex> vertex;
  gl::Array<betr::Vec2> base_vertex;
  gl::VertexArray vao;

  ShaderRegistry &registry;
  GLuint texture;
  int count;

  void init();
};
