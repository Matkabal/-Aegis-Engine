#include "engine/runtime/camera.h"

#include <algorithm>
#include <cmath>

namespace engine::runtime {

void Camera::update(float dt_seconds, const input::InputState& input) {
  update_orientation(input);

  float speed = move_speed;
  if (input.isDown(input::Key::LeftShift) || input.isDown(input::Key::RightShift)) {
    speed *= fast_multiplier;
  }

  const float step = speed * dt_seconds;
  if (input.isDown(input::Key::W)) {
    position = position + (forward * step);
  }
  if (input.isDown(input::Key::S)) {
    position = position - (forward * step);
  }
  if (input.isDown(input::Key::D)) {
    position = position + (right * step);
  }
  if (input.isDown(input::Key::A)) {
    position = position - (right * step);
  }

  update_view();
  if (projection_dirty_) {
    update_projection();
  }
}

void Camera::set_viewport(const int width, const int height) {
  viewport_width_ = std::max(1, width);
  viewport_height_ = std::max(1, height);
  projection_dirty_ = true;
}

void Camera::update_orientation(const input::InputState& input) {
  const math::Vec2 delta = input.mouseDelta();
  yaw_ += delta.x * mouse_sensitivity;
  pitch_ -= delta.y * mouse_sensitivity;
  pitch_ = std::clamp(pitch_, -1.55334F, 1.55334F);

  const float cos_pitch = std::cos(pitch_);
  forward = math::normalize({
      std::cos(yaw_) * cos_pitch,
      std::sin(pitch_),
      std::sin(yaw_) * cos_pitch,
  });

  constexpr math::Vec3 world_up{0.0F, 1.0F, 0.0F};
  right = math::normalize(math::cross(forward, world_up));
  up = math::normalize(math::cross(right, forward));
}

void Camera::update_view() {
  view = math::look_at(position, position + forward, up);
}

void Camera::update_projection() {
  const float aspect = static_cast<float>(viewport_width_) / static_cast<float>(viewport_height_);
  projection = math::perspective(fov_radians, aspect, near_plane, far_plane);
  projection_dirty_ = false;
}

} // namespace engine::runtime
