#version 330 core
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(std140, binding = 1) uniform Visual {
  vec4 color;
} visual;

out vec4 color;

void main() {
  color = visual.color;
}
