#pragma once

#include <glad/gl.h>

#include <betr/atomic.hpp>
#include <betr/init_list.hpp>

#include <iostream>

extern void render();

template <typename T> class Buffer {
public:
  Buffer(GLenum type, const betr::InitList<T> data, GLenum usage = GL_STATIC_DRAW) : type(type), usage(usage) {
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, data.size() * sizeof(T), data.begin(), usage);
  };

  void bind() const noexcept { glBindBuffer(type, id); }
  void unbind() const noexcept { glBindBuffer(type, 0); }

  operator GLuint() const noexcept { return this->id; }

  ~Buffer() { glDeleteBuffers(1, &id); }

  Buffer(Buffer &&) = delete;
  Buffer(const Buffer &) = delete;
  Buffer &operator=(Buffer &&) = delete;
  Buffer &operator=(const Buffer &) = delete;

protected:
  const GLenum type;
  const GLenum usage;
  GLuint id;
};

template <typename T> class DynamicBuffer final : public Buffer<T> {};

class VertexArray {
public:
  VertexArray() {
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);
  };

  void bind_buffer(GLuint buffer, GLsizei stride, GLuint divisor = 0) const noexcept {
    glBindVertexBuffer(buffer, buffer, 0, stride);
    glVertexBindingDivisor(buffer, divisor);
  }

  void add_attib(GLuint buffer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset) const noexcept {
    glVertexAttribFormat(index, size, type, normalized, offset);
    glEnableVertexAttribArray(index);
    glVertexAttribBinding(index, buffer);
  }

  void add_integer_attib(GLuint buffer, GLuint index, GLint size, GLenum type, GLuint offset) const noexcept {
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

class Program {
public:
  Program(GLenum type, const char *source) {
    GLint success;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      GLint lenght;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &lenght);
      auto log = new char[lenght];

      glGetShaderInfoLog(shader, lenght, NULL, log);
      std::cerr << "Shader Compilation Failed" << std::endl << log << std::endl;

      delete[] log;
      id = 0;
    }

    GLuint program = glCreateProgram();
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
      GLint length;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
      auto log = new char[length];

      glGetProgramInfoLog(program, length, NULL, log);
      std::cerr << "Program Linking Failed" << std::endl << log << std::endl;

      delete[] log;
      id = 0;
    }

    glDeleteShader(shader);
    id = program;
  }

  operator GLuint() const noexcept { return id; }
  ~Program() { glDeleteProgram(id); }

  Program(Program &&) = delete;
  Program(const Program &) = delete;
  Program &operator=(Program &&) = delete;
  Program &operator=(const Program &) = delete;

private:
  GLuint id;
};

class Pipeline {
public:
  Pipeline() {
    glGenProgramPipelines(1, &id);
    glBindProgramPipeline(id);
  };

  void use_stages(GLbitfield stages, GLuint program) const noexcept { glUseProgramStages(id, stages, program); }

  operator GLuint() const noexcept { return id; }
  ~Pipeline() { glDeleteProgramPipelines(1, &id); };

  Pipeline(Pipeline &&) = delete;
  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(Pipeline &&) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

private:
  GLuint id;
};
