#include "shader.hpp"

#include <betr/chrono.hpp>
#include <betr/filesystem.hpp>
#include <betr/namespace.hpp>

#include <fstream>
#include <glad/gl.h>
#include <print>

void ShaderRegistry::load_shader(const char *name) {
  Path path = m_dir + "/" + name + ".glsl";

  if (!is_regular_file(path)) { return; }
  const auto size = file_size(path);

  GLint type;

  if (String(name).find(".vert") != String::npos) {
    type = GL_VERTEX_SHADER;
  } else if (String(name).find(".frag") != String::npos) {
    type = GL_FRAGMENT_SHADER;
  } else {
    return;
  }

  std::ifstream in(path);
  shader_src.resize(size);
  in.read(shader_src.data(), size);
  const char *c_shader_src = shader_src.data();

  GLint success;

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &c_shader_src, NULL);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint lenght;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &lenght);
    auto log = new char[lenght];

    glGetShaderInfoLog(shader, lenght, NULL, log);
    std::println("{} Compilation Failed", name);
    std::println("{}", log);

    delete[] log;
    glDeleteShader(shader);
    return;
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
    std::println("{} Linking Failed", name);
    std::println("{}", log);

    delete[] log;
    glDeleteShader(shader);
    glDeleteProgram(program);
    return;
  }
  glDeleteShader(shader);

  programs[name] = program;
}

ShaderRegistry::ShaderRegistry(const String dir) : m_dir(dir) {
  reload_time = FileClock::now().time_since_epoch().count();
}

void ShaderRegistry::reload() {
  for (auto item : programs) {
    const char *name = item.first;
    const long write_time = fs::last_write_time(m_dir + "/" + name + ".glsl").time_since_epoch().count();

    if (write_time <= reload_time) { continue; }

    std::println("Reloading: {}", name);
    glDeleteProgram(programs[name]);

    load_shader(name);
  }

  reload_time = FileClock::now().time_since_epoch().count();
}

ShaderRegistry::~ShaderRegistry() {
  for (const auto &item : programs) { glDeleteProgram(item.second); }
}
