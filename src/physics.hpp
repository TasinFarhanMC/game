#pragma once
#include <render/quads.hpp>

struct Collider {
  Quads::Vertex &vertex;

  betr::Vec2 vel;
  betr::Vec2 &pos = vertex.pos;
  betr::UVec2 &scale = vertex.scale;

  Collider(Quads::Vertex &vertex, betr::Vec2 vel = {}) : vertex(vertex), vel(vel) {}

  bool colliding(Collider &b, float delta_t);

  float bottom() const { return pos.y; }
  float left() const { return pos.x; }
  float top() const { return pos.y + scale.y; }
  float right() const { return pos.x + scale.x; }

  void bottom(float y) const { pos.y = y; }
  void left(float x) const { pos.x = x; }
  void top(float y) const { pos.y = y - scale.y; }
  void right(float x) const { pos.x = x - scale.x; }

  void bounce_x(float wall, float e);
  void bounce_y(float wall, float e);
};
