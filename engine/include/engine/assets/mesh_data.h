#pragma once

#include "engine/math/vec3.h"

#include <cstdint>
#include <vector>

namespace engine::assets {

struct Aabb {
  math::Vec3 min{0.0F, 0.0F, 0.0F};
  math::Vec3 max{0.0F, 0.0F, 0.0F};
};

struct Vertex {
  math::Vec3 position;
};

struct MeshData {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  Aabb bounds;
};

Aabb compute_aabb(const std::vector<Vertex>& vertices);

} // namespace engine::assets
