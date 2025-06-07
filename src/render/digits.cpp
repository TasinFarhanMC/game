#include "digits.hpp"
#include "gl.hpp"
#include <betr/namespace.hpp>
#include <print>
#include <stb_image.h>

using namespace gl;

void Digits::init() {
  vao.bind();
  vao.bind_buffer(base_vertex, sizeof(Vec2));
  vao.add_attib(base_vertex, 0, 2, GL_FLOAT, false, 0);

  vao.bind_buffer(vertex, sizeof(Vertex), 1);
  vao.add_attib(vertex, 1, 2, GL_FLOAT, false, offsetof(Vertex, pos));
  vao.add_iattib(vertex, 2, 2, GL_UNSIGNED_INT, offsetof(Vertex, scale));
  vao.add_iattib(vertex, 3, 1, GL_UNSIGNED_BYTE, offsetof(Vertex, id));

  stbi_set_flip_vertically_on_load(true);
  int width, height, nrChannels;
  unsigned char *data = stbi_load("assets/fonts/bitmap.png", &width, &height, &nrChannels, 0);
  std::println("Channels {}", nrChannels);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  GLenum format = GL_RED;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Digits::render(Pipeline &pipeline) {
  GLuint frag = registry["font.frag"];

  pipeline.use_vert(registry["font.vert"]);
  pipeline.use_frag(frag);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glProgramUniform1i(frag, glGetUniformLocation(frag, "tex"), 0);

  vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, count);
}
