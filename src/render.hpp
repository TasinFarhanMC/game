#pragma once

#include <glad/gl.h>

#include <betr/atomic.hpp>

#include <initializer_list>
#include <iostream>

extern void render();

class Buffer {
  GLuint id;

  Buffer(Buffer &&) = delete;
  Buffer(const Buffer &) = delete;
  Buffer &operator=(Buffer &&) = delete;
  Buffer &operator=(const Buffer &) = delete;

public:
  template <typename T> Buffer(GLenum type, const std::initializer_list<T> data, GLenum usage) {
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, data.size() * sizeof(T), data.begin(), usage);
  };

  operator GLuint() const noexcept { return id; }

  ~Buffer() { glDeleteBuffers(1, &id); };
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
