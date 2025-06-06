#version 330 core
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec2 base_pos;
layout(location = 1) in vec2 pos;
layout(location = 2) in uvec2 scale;
layout(location = 3) in uint id;

layout(std140, binding = 0) uniform Simulation {
  vec2 dim;
} sim;

out RTX {
  vec2 tex_coord;
  flat uint tex_id;
} tx;

void main() {
  tx.tex_id = id;
  tx.tex_coord = base_pos;

  const vec2 world_pos = base_pos * scale + pos;
  gl_Position = vec4(world_pos * 2 / sim.dim - 1, 0.0f, 1.0f);
}
