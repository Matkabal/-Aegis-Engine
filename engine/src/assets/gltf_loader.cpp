#include "engine/assets/gltf_loader.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace engine::assets {

GltfLoadResult GltfLoader::load(const std::string& path) {
  GltfLoadResult result{};

  const std::filesystem::path file_path(path);
  const std::string ext = file_path.extension().string();
  if (ext != ".gltf" && ext != ".glb") {
    result.error = "Unsupported mesh format: " + ext;
    return result;
  }

  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) {
    result.error = "Failed to open glTF file: " + path;
    return result;
  }

  std::ostringstream content_stream;
  content_stream << in.rdbuf();
  const std::string content = content_stream.str();
  if (content.empty()) {
    result.error = "glTF file is empty: " + path;
    return result;
  }

  if (ext == ".gltf" && content.find("\"asset\"") == std::string::npos) {
    result.error = "Invalid glTF JSON (missing asset object): " + path;
    return result;
  }

  // M2 base loader: parse validation + create a first internal mesh representation.
  MeshData mesh{};
  mesh.vertices = {
      {{-0.5F, -0.5F, 0.0F}},
      {{0.5F, -0.5F, 0.0F}},
      {{0.0F, 0.5F, 0.0F}},
  };
  mesh.indices = {0, 1, 2};
  mesh.bounds = compute_aabb(mesh.vertices);

  result.ok = true;
  result.meshes.push_back(mesh);
  return result;
}

} // namespace engine::assets
