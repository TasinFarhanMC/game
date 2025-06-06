#version 330 core
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable

in RTX {
  vec2 tex_coord;
  flat uint tex_id;
} rx;

out vec4 color;

uniform sampler2D tex;

void main() {
float glyph_width_px = 4.0;
float tex_width_px   = 40.0;
float uv_per_px      = 1.0 / tex_width_px;

float glyph_start_u = float(rx.tex_id) * glyph_width_px * uv_per_px;
float glyph_u       = glyph_start_u + rx.tex_coord.x * (3.0 * uv_per_px);

color = texture(tex, vec2(glyph_u, rx.tex_coord.y));
}
