#pragma once

#include <betr/init_list.hpp>
#include <glad/gl.h>
#include <print>

namespace gl {
template <typename T> class Buffer {
public:
  Buffer(GLenum type, const betr::InitList<T> data) : type(type) {
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, data.size() * sizeof(T), data.begin(), GL_DYNAMIC_DRAW);
  };
  ~Buffer() { glDeleteBuffers(1, &id); }

  operator GLuint() const noexcept { return this->id; }

  T *map() {
    glBindBuffer(type, id);
    return static_cast<T *>(glMapBuffer(type, GL_READ_WRITE));
  }
  void unmap() { glUnmapBuffer(type); }

  void update(const betr::InitList<T> data) {
    glBindBuffer(type, id);
    glBufferData(type, data.size(), data.data(), GL_DYNAMIC_DRAW);
  }
  void update(const betr::InitList<T> data, const GLintptr offset) {
    glBindBuffer(type, id);
    glBufferSubDataData(type, offset, data.size(), data.data());
  }
  void update(const betr::InitList<T> data, const GLintptr offset, const GLintptr size) {
    glBindBuffer(type, id);
    glBufferSubDataData(type, offset, size, data.data());
  }

private:
  const GLenum type;
  GLuint id;
};

template <typename T> class Array {
public:
  Array(GLenum type, const betr::InitList<T> data) : type(type) {
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, data.size() * sizeof(T), data.begin(), GL_STATIC_DRAW);
  };
  ~Array() { glDeleteBuffers(1, &id); }

  operator GLuint() const noexcept { return this->id; }

  T *map() {
    glBindBuffer(type, id);
    return static_cast<T *>(glMapBuffer(type, GL_READ_ONLY));
  }
  void unmap() { glUnmapBuffer(type); }

  Array(Array &&) = delete;
  Array(const Array &) = delete;
  Array &operator=(Array &&) = delete;
  Array &operator=(const Array &) = delete;

private:
  const GLenum type;
  GLuint id;
};

class VertexArray {
public:
  VertexArray() {
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);
  };

  void bind() { glBindVertexArray(id); }

  void bind_buffer(GLuint buffer, GLsizei stride, GLuint divisor = 0) const noexcept {
    glBindVertexBuffer(buffer, buffer, 0, stride);
    glVertexBindingDivisor(buffer, divisor);
  }

  void add_attib(GLuint buffer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset)
      const noexcept {
    glVertexAttribFormat(index, size, type, normalized, offset);
    glEnableVertexAttribArray(index);
    glVertexAttribBinding(index, buffer);
  }

  void add_iattib(GLuint buffer, GLuint index, GLint size, GLenum type, GLuint offset) const noexcept {
    glVertexAttribIFormat(index, size, type, offset);
    glEnableVertexAttribArray(index);
    glVertexAttribBinding(index, buffer);
  }

  operator GLuint() const noexcept { return id; }
  ~VertexArray() { glDeleteBuffers(1, &id); };

  VertexArray(VertexArray &&) = delete;
  VertexArray(const VertexArray &) = delete;
  VertexArray &operator=(VertexArray &&) = delete;
  VertexArray &operator=(const VertexArray &) = delete;

private:
  GLuint id;
};

class Pipeline {
public:
  Pipeline() {
    glGenProgramPipelines(1, &id);
    glBindProgramPipeline(id);
  };

  void bind() { glBindProgramPipeline(id); }

  void use_vert(GLuint program) const noexcept { glUseProgramStages(id, GL_VERTEX_SHADER_BIT, program); }
  void use_frag(GLuint program) const noexcept { glUseProgramStages(id, GL_FRAGMENT_SHADER_BIT, program); }

  operator GLuint() const noexcept { return id; }
  ~Pipeline() { glDeleteProgramPipelines(1, &id); };

  Pipeline(Pipeline &&) = delete;
  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(Pipeline &&) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

private:
  GLuint id;
};

template <typename T> class UniformBuffer {
  GLuint ubo = 0;
  GLuint binding;

public:
  UniformBuffer(GLuint binding, const T &initial) : binding(binding) {
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &initial, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
  }

  void use() { glBindBuffer(GL_UNIFORM_BUFFER, ubo); }
  void set(const T &value) { glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &value); }

  T *map() { return static_cast<T *>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE)); }
  void unmap() { glUnmapBuffer(GL_UNIFORM_BUFFER); }

  ~UniformBuffer() { glDeleteBuffers(1, &ubo); }
};
}; // namespace gl
