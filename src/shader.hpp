#pragma once
#include <betr/string.hpp>
#include <betr/unordered_map.hpp>

class ShaderRegistry {
public:
  ShaderRegistry(const betr::String dir);
  void reload();

  unsigned int operator[](const char *name) {
    if (!programs.contains(name)) { load_shader(name); }
    return programs[name];
  }

  ~ShaderRegistry();

  ShaderRegistry(ShaderRegistry &&) = delete;
  ShaderRegistry(const ShaderRegistry &) = delete;
  ShaderRegistry &operator=(ShaderRegistry &&) = delete;
  ShaderRegistry &operator=(const ShaderRegistry &) = delete;

private:
  const betr::String m_dir;
  long reload_time;
  betr::String shader_src;
  betr::UnorderedMap<const char *, unsigned int> programs;

  void load_shader(const char *name);
};
