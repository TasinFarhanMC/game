#include "physics.hpp"
#include <betr/namespace.hpp>
#include <cmath>

bool Collider::colliding(Collider &b, float delta_t) {
  // Calculate expanded static box (b)
  Vec2 b_min = b.pos;
  Vec2 b_max = b.pos + Vec2(b.scale);

  // Expand static box by our () size
  Vec2 expanded_min = b_min - Vec2(scale);
  Vec2 expanded_max = b_max;

  // Ray direction (velocity)
  Vec2 vel_dt = vel * delta_t;

  Vec2 inv_entry, inv_exit;

  for (int i = 0; i < 2; ++i) {
    if (vel_dt[i] > 0) {
      inv_entry[i] = (expanded_min[i] - pos[i]) / vel_dt[i];
      inv_exit[i] = (expanded_max[i] - pos[i]) / vel_dt[i];
    } else if (vel_dt[i] < 0) {
      inv_entry[i] = (expanded_max[i] - pos[i]) / vel_dt[i];
      inv_exit[i] = (expanded_min[i] - pos[i]) / vel_dt[i];
    } else {
      inv_entry[i] = -INFINITY;
      inv_exit[i] = INFINITY;
    }
  }

  float entry_time = std::max(inv_entry.x, inv_entry.y);
  float exit_time = std::min(inv_exit.x, inv_exit.y);

  return (entry_time <= exit_time) && (entry_time >= 0.0f) && (entry_time <= 1.0f);
}

void Collider::bounce_x(float wall, float e) {
  pos.x = wall + e * (pos.x - wall);
  vel.x *= -e;
}

void Collider::bounce_y(float wall, float e) {
  pos.y = wall + e * (pos.y - wall);
  vel.y *= -e;
}
