#pragma once

inline constexpr const char *vert_src = R"(
  #version 330 core
  #extension GL_ARB_shading_language_420pack  : enable
  #extension GL_ARB_explicit_uniform_location : enable

  layout(location = 0) in vec2 basePos;
  layout(location = 1) in vec2 aPos;
  layout(location = 2) in uvec2 aScale;

  const vec4 ortho = {256.0f, 256.0f, 0.0f, 0.0f};

  void main() {
    const vec2 world_pos = basePos * (aScale + vec2(1)) + aPos;
    gl_Position = vec4((world_pos * 2.0f - ortho.xy - ortho.zw) / (ortho.xy - ortho.zw), 0.0f, 1.0f);
  }
)";

inline constexpr const char *frag_src = R"(
  #version 330 core
  #extension GL_ARB_shading_language_420pack  : enable
  #extension GL_ARB_explicit_uniform_location : enable

  out vec4 color;

  void main() {
    color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  }
)";
