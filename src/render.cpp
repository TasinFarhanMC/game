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

struct IndirectCMD {
  GLuint count;
  GLuint instance_Count;
  GLuint first;
  GLuint base_instance;
};

void render() {
  {
    if (!load_context()) {
      std::cerr << "Failed to load GL context" << std::endl;
      return;
    }

    if (!gladLoaderLoadGL()) {
      std::cerr << "Unable to init GLAD" << std::endl;
      return;
    }

    VertexArray vertex_array;

    Buffer<float> base_vertex(GL_ARRAY_BUFFER, {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f});
    glBindVertexBuffer(0, base_vertex, 0, sizeof(Vec2));

    DynamicBuffer<Vertex> vertex(GL_ARRAY_BUFFER, {{160.0f - 16, 90.0f - 16, 32, 32, 0, 0}});

    vertex_array.bind_buffer(base_vertex, sizeof(Vec2));
    vertex_array.add_attib(base_vertex, 0, 2, GL_FLOAT, false, 0);

    vertex_array.bind_buffer(vertex, sizeof(Vertex), 1);
    vertex_array.add_attib(vertex, 1, 2, GL_FLOAT, false, offset_of(Vertex, position));
    vertex_array.add_integer_attib(vertex, 2, 2, GL_UNSIGNED_BYTE, offset_of(Vertex, scale));

    Program vert_program(GL_VERTEX_SHADER, vert_src);
    Program frag_program(GL_FRAGMENT_SHADER, frag_src);

    Pipeline pipeline;
    pipeline.use_stages(GL_VERTEX_SHADER_BIT, vert_program);
    pipeline.use_stages(GL_FRAGMENT_SHADER_BIT, frag_program);

    DynamicBuffer<IndirectCMD> indirect(GL_DRAW_INDIRECT_BUFFER, {{4, 1, 0, 0}});

    while (running.test()) {
      glClear(GL_COLOR_BUFFER_BIT);
      glMultiDrawArraysIndirect(GL_TRIANGLE_FAN, 0, indirect.data.size(), 0);
      swap_buffers();
    }
  }

  unload_context();
}
