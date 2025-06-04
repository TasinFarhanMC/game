#version 330 core
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(std140, binding = 0) uniform Frag {
  vec4 uColor;
};

out vec4 color;

void main() {
  color = uColor;
}
