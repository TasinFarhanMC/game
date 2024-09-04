#pragma once

#include <string.hpp>
#include <glad/gl.h>
#include <io.hpp>
#include <vector.hpp>

class ShaderParser {
  IFStream file_stream;
  struct Data {
    String source;
    GLuint type;
  };
  Vector<Data> data;

public:
  ShaderParser(ShaderParser &&) = delete;
  ShaderParser(const ShaderParser &) = delete;
  ShaderParser &operator=(ShaderParser &&) = delete;
  ShaderParser &operator=(const ShaderParser &) = delete;

  const Vector<Data> &get_data() const noexcept { return data; }
  [[nodiscard]] bool parse();

  ShaderParser(IFStream &&file_stream) : file_stream(std::move(file_stream)) {};
  ShaderParser(const String &file_name) : file_stream(file_name) {};
};

class Shader {
  GLuint id;

public:
  Shader(Shader &&) = delete;
  Shader(const Shader &) = delete;
  Shader &operator=(Shader &&) = delete;
  Shader &operator=(const Shader &) = delete;

  const String name, source;
  const GLuint type;

  GLuint get_id() const noexcept { return id; }
  [[nodiscard]] bool compile();
  ~Shader();

  explicit Shader(const String name, const String source, const GLuint type) : name(std::move(name)), source(std::move(source)), type(type), id(0) {};
};

class ShaderProgram {
  GLuint id;

public:
  const Vector<Shader> shaders;

  ShaderProgram(ShaderProgram &&) = delete;
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(ShaderProgram &&) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;

  GLuint get_id() const noexcept { return id; }
  [[nodiscard]] bool compile();
  ~ShaderProgram();

  explicit ShaderProgram(Vector<Shader> shaders) : shaders(std::move(shaders)), id(0) {}
};
