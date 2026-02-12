#pragma once

#include "engine/math/mat4.h"
#include "engine/math/vec3.h"

namespace engine::runtime {

class Transform {
public:
  math::Vec3 position{0.0F, 0.0F, 0.0F};
  math::Vec3 rotation{0.0F, 0.0F, 0.0F};
  math::Vec3 scale{1.0F, 1.0F, 1.0F};
  math::Mat4 world_matrix = math::identity();

  void mark_dirty();
  void update_matrix();

private:
  bool dirty_ = true;
};

} // namespace engine::runtime
