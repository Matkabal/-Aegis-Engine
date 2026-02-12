#pragma once

#include "engine/assets/mesh_data.h"

#include <string>
#include <vector>

namespace engine::assets {

struct GltfLoadResult {
  bool ok = false;
  std::vector<MeshData> meshes;
  std::string error;
};

class GltfLoader {
public:
  static GltfLoadResult load(const std::string& path);
};

} // namespace engine::assets
