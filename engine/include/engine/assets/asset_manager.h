#pragma once

#include "engine/assets/mesh_data.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace engine::core {
class Logger;
}

namespace engine::assets {

struct MeshHandle {
  uint32_t value = 0;

  bool valid() const { return value != 0; }
};

class AssetManager {
public:
  explicit AssetManager(core::Logger* logger = nullptr);

  MeshHandle load_mesh(const std::string& path);
  const MeshData* get_mesh(MeshHandle handle) const;
  bool unload_mesh(MeshHandle handle);

  uint32_t mesh_count() const;

private:
  void log_info(const std::string& message) const;
  void log_warn(const std::string& message) const;
  void log_error(const std::string& message) const;

  core::Logger* logger_ = nullptr;
  uint32_t next_handle_ = 1;

  std::unordered_map<std::string, MeshHandle> path_cache_;
  std::unordered_map<uint32_t, MeshData> meshes_;
};

} // namespace engine::assets
