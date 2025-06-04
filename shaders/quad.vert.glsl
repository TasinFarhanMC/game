#version 330 core
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec2 basePos;
layout(location = 1) in vec2 aPos;
layout(location = 2) in uvec2 aScale;

const vec2 ortho = 2.0f / vec2(100.0f, 100.0f);

void main() {
  const vec2 world_pos = basePos * aScale + aPos;
  gl_Position = vec4(world_pos * ortho - 1, 0.0f, 1.0f);
}
