#pragma once

namespace Shader {
void load(const char *dir);
unsigned get(const char *name);
void reload();
void unload();
}; // namespace Shader
