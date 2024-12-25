#pragma once

#include <glad/gl.h>

#include <betr/atomic.hpp>
#include <betr/init_list.hpp>
#include <betr/vector.hpp>

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

protected:
  const GLenum type;
  const GLenum usage;
  GLuint id;

private:
  Buffer(Buffer &&) = delete;
  Buffer(const Buffer &) = delete;
  Buffer &operator=(Buffer &&) = delete;
  Buffer &operator=(const Buffer &) = delete;
};

template <typename T> class DynamicBuffer final : public Buffer<T> {
public:
  DynamicBuffer(GLenum type, const std::initializer_list<T> data, GLenum usage = GL_DYNAMIC_DRAW) : Buffer<T>(type, data, usage), data(data) {}
  DynamicBuffer(GLenum type, const std::initializer_list<T> data, const size_t size, GLenum usage = GL_DYNAMIC_DRAW) : Buffer<T>(type, {}, usage) {
    this->data.reserve(size);
    this->data = data;
  }

  void update() const {
    glBindBuffer(this->type, this->id);
    glBufferData(this->type, this->data.size() * sizeof(T), this->data.data(), this->usage);
  }

  betr::Vector<T> data;
};

class VertexArray {
  GLuint id;

  VertexArray(VertexArray &&) = delete;
  VertexArray(const VertexArray &) = delete;
  VertexArray &operator=(VertexArray &&) = delete;
  VertexArray &operator=(const VertexArray &) = delete;

public:
  VertexArray() { glGenVertexArrays(1, &id); };
  ~VertexArray() { glDeleteBuffers(1, &id); };

  operator GLuint() const noexcept { return id; }
};

class Program {
  GLuint id;

  Program(Program &&) = delete;
  Program(const Program &) = delete;
  Program &operator=(Program &&) = delete;
  Program &operator=(const Program &) = delete;

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
};
