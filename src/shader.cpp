#include "shader.hpp"

#include <betr/chrono.hpp>
#include <betr/filesystem.hpp>
#include <betr/namespace.hpp>
#include <betr/string.hpp>
#include <betr/unordered_map.hpp>

#include <fstream>
#include <print>

#include <glad/gl.h>

static UnorderedMap<const char *, unsigned int> map;
static long reload_time;
static String dir;

namespace Shader {
void load(const char *dir) {
  ::dir = dir;
  reload_time = FileClock::now().time_since_epoch().count();
}

unsigned add_shader(const char *name) {
  static String shader_src;

  Path path = dir + "/" + name + ".glsl";

  if (!is_regular_file(path)) { return 0; }
  const auto size = file_size(path);

  GLint type;

  if (String(name).find(".vert") != String::npos) {
    type = GL_VERTEX_SHADER;
  } else if (String(name).find(".frag") != String::npos) {
    type = GL_FRAGMENT_SHADER;
  } else {
    return 0;
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
    return 0;
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
    return 0;
  }
  glDeleteShader(shader);

  map[name] = program;
  return program;
}

unsigned get(const char *name) {
  if (!map.contains(name)) { return add_shader(name); }
  return map[name];
}

void reload() {
  for (auto [name, id] : map) {
    const long write_time = fs::last_write_time(dir + "/" + name + ".glsl").time_since_epoch().count();
    if (write_time <= reload_time) { continue; }

    std::println("Reloading: {}", name);
    glDeleteProgram(id);

    add_shader(name);
  }

  reload_time = FileClock::now().time_since_epoch().count();
}

void unload() {
  for (const auto [_, shader] : map) { glDeleteProgram(shader); }
}
} // namespace Shader
