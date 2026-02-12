#include "engine/assets/mesh_data.h"

#include <algorithm>

namespace engine::assets {

Aabb compute_aabb(const std::vector<Vertex>& vertices) {
  if (vertices.empty()) {
    return {};
  }

  Aabb out{};
  out.min = vertices[0].position;
  out.max = vertices[0].position;

  for (const Vertex& v : vertices) {
    out.min.x = std::min(out.min.x, v.position.x);
    out.min.y = std::min(out.min.y, v.position.y);
    out.min.z = std::min(out.min.z, v.position.z);

    out.max.x = std::max(out.max.x, v.position.x);
    out.max.y = std::max(out.max.y, v.position.y);
    out.max.z = std::max(out.max.z, v.position.z);
  }

  return out;
}

} // namespace engine::assets
