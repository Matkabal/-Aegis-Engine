#include "engine/assets/asset_manager.h"

#include "engine/assets/gltf_loader.h"
#include "engine/core/logger.h"

namespace engine::assets {

AssetManager::AssetManager(core::Logger* logger)
    : logger_(logger) {}

MeshHandle AssetManager::load_mesh(const std::string& path) {
  if (const auto it = path_cache_.find(path); it != path_cache_.end()) {
    log_info("Asset cache hit for mesh: " + path);
    return it->second;
  }

  const GltfLoadResult load_result = GltfLoader::load(path);
  if (!load_result.ok || load_result.meshes.empty()) {
    log_error("Failed to load mesh '" + path + "': " + load_result.error);
    return {};
  }

  const MeshHandle handle{next_handle_++};
  meshes_[handle.value] = load_result.meshes[0];
  path_cache_[path] = handle;

  log_info("Loaded mesh from path: " + path);
  return handle;
}

const MeshData* AssetManager::get_mesh(const MeshHandle handle) const {
  if (!handle.valid()) {
    return nullptr;
  }

  const auto it = meshes_.find(handle.value);
  if (it == meshes_.end()) {
    return nullptr;
  }

  return &it->second;
}

bool AssetManager::unload_mesh(const MeshHandle handle) {
  if (!handle.valid()) {
    return false;
  }

  const auto it = meshes_.find(handle.value);
  if (it == meshes_.end()) {
    return false;
  }

  meshes_.erase(it);

  for (auto path_it = path_cache_.begin(); path_it != path_cache_.end();) {
    if (path_it->second.value == handle.value) {
      path_it = path_cache_.erase(path_it);
    } else {
      ++path_it;
    }
  }

  return true;
}

uint32_t AssetManager::mesh_count() const {
  return static_cast<uint32_t>(meshes_.size());
}

void AssetManager::log_info(const std::string& message) const {
  if (logger_ != nullptr) {
    logger_->info(message);
  }
}

void AssetManager::log_warn(const std::string& message) const {
  if (logger_ != nullptr) {
    logger_->warn(message);
  }
}

void AssetManager::log_error(const std::string& message) const {
  if (logger_ != nullptr) {
    logger_->error(message);
  }
}

} // namespace engine::assets
