#include "engine/runtime/transform.h"

namespace engine::runtime {

void Transform::mark_dirty() {
  dirty_ = true;
}

void Transform::update_matrix() {
  if (!dirty_) {
    return;
  }

  world_matrix = math::trs(position, rotation, scale);
  dirty_ = false;
}

} // namespace engine::runtime
