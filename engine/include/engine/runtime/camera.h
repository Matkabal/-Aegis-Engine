#pragma once

#include "engine/input/input_state.h"
#include "engine/math/mat4.h"
#include "engine/math/vec3.h"

namespace engine::runtime {

class Camera {
public:
  math::Vec3 position{0.0F, 0.0F, 3.0F};
  math::Vec3 forward{0.0F, 0.0F, -1.0F};
  math::Vec3 right{1.0F, 0.0F, 0.0F};
  math::Vec3 up{0.0F, 1.0F, 0.0F};

  float fov_radians = 1.0471975512F; // 60 deg
  float near_plane = 0.1F;
  float far_plane = 200.0F;

  float move_speed = 4.0F;
  float fast_multiplier = 3.0F;
  float mouse_sensitivity = 0.003F;

  math::Mat4 view = math::identity();
  math::Mat4 projection = math::identity();

  void update(float dt_seconds, const input::InputState& input);
  void set_viewport(int width, int height);

private:
  void update_orientation(const input::InputState& input);
  void update_view();
  void update_projection();

  float yaw_ = -1.57079632679F;
  float pitch_ = 0.0F;
  int viewport_width_ = 1280;
  int viewport_height_ = 720;
  bool projection_dirty_ = true;
};

} // namespace engine::runtime
