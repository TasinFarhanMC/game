#pragma once
#include <betr/string.hpp>
#include <betr/unordered_map.hpp>

class ShaderReg {
public:
  ShaderReg(const betr::String dir);
  void reload();

  unsigned int operator[](const char *name) {
    if (!programs.contains(name)) { load_shader(name); }
    return programs[name];
  }

  ~ShaderReg();

  ShaderReg(ShaderReg &&) = delete;
  ShaderReg(const ShaderReg &) = delete;
  ShaderReg &operator=(ShaderReg &&) = delete;
  ShaderReg &operator=(const ShaderReg &) = delete;

private:
  const betr::String m_dir;
  long reload_time;
  betr::String shader_src;
  betr::UnorderedMap<const char *, unsigned int> programs;

  void load_shader(const char *name);
};
