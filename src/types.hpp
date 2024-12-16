#include "glad/gl.h"

#include <atomic>
#include <functional>
#include <glm/glm.hpp>
#include <initializer_list>
#include <iostream>
#include <thread>

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using usize = std::size_t;
using Vec2 = glm::vec2;
using IVec2 = glm::ivec2;
using U8Vec2 = glm::u8vec2;
using Thread = std::thread;
template <typename T> using InitList = std::initializer_list<T>;
template <typename T> using Atomic = std::atomic<T>;
template <typename Signature> using Function = std::function<Signature>;

struct Vertex {
  Vec2 position;
  U8Vec2 scale;
  U8Vec2 tex;

  Vertex(float x, float y, u8 w, u8 h, u8 tex_no, u8 tex_id) : position(x, y), scale(w, h), tex(tex_no, tex_id) {}
};

class Flag {
  Atomic<bool> flag;

public:
  Flag(const bool value) : flag(value) {}

  bool operator=(const bool value) { return flag.exchange(value, std::memory_order_acq_rel); }
  operator bool() const { return flag.load(std::memory_order_acquire); }
};

template <typename T> class Signal {
  Flag flag = false;
  Atomic<T> value;

public:
  void set(const T value) {
    flag = true;
    this->value.store(value, std::memory_order_release);
  }
  void set(const Function<T(const T)> mutate) {
    flag = true;
    value.store(mutate(value.load(std::memory_order_acquire)), std::memory_order_release);
  }
  void handle(const Function<void(const T)> handler) {
    if (flag) { handler(value.load(std::memory_order_acquire)); }
  }

  Signal(const T value) { this->value.store(value, std::memory_order_release); }
};

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
