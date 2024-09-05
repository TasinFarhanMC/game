#pragma once

#include <fstream.hpp>
#include <glad/gl.h>
#include <string.hpp>
#include <vector.hpp>

class ShaderParser {
public:
  struct Data {
    String source;
    GLuint type;
  };

  ShaderParser(ShaderParser &&) = delete;
  ShaderParser(const ShaderParser &) = delete;
  ShaderParser &operator=(ShaderParser &&) = delete;
  ShaderParser &operator=(const ShaderParser &) = delete;

  const Vector<Data> &get_data() const noexcept { return data; }
  [[nodiscard]] bool parse();

  ShaderParser(IFStream &&file_stream) : file_stream(std::move(file_stream)) {};
  ShaderParser(const String &file_name) : file_stream(file_name) {};

private:
  Vector<Data> data;
  IFStream file_stream;
};

class Shader {
  GLuint id;

public:
  Shader(Shader &&) = delete;
  Shader(const Shader &) = delete;
  Shader &operator=(Shader &&) = delete;
  Shader &operator=(const Shader &) = delete;

  const char *name;
  const char *source;
  const char *type_name;
  const GLuint type;

  void compile();
  GLuint get_id() const noexcept { return id; }

  ~Shader() {
    if (id) { glDeleteShader(id); }
  };

  explicit Shader(const String name, const String source, const GLuint type, const bool compile = false)
      : name(std::move(name.c_str())),
        source(std::move(source.c_str())),
        type_name(get_type_name(type)),
        type(type),
        id(0) {
    if (compile) { this->compile(); }
  };

  static const char *const get_type_name(GLint type) {
    switch (type) {
    case GL_VERTEX_SHADER:
      return "Vertex";
    case GL_FRAGMENT_SHADER:
      return "Fragment";
    case GL_GEOMETRY_SHADER:
      return "Geometry";
    default:
      return "Unknown";
    }
  }
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
  void compile();

  ~ShaderProgram() {
    if (id) { glDeleteProgram(id); }
  };

  explicit ShaderProgram(Vector<Shader> shaders) : shaders(std::move(shaders)), id(0) {}
};
