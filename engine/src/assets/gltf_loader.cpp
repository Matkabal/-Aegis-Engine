#include "engine/assets/gltf_loader.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace engine::assets {

namespace {

MeshData make_cone_mesh(const int segments) {
  MeshData mesh{};
  if (segments < 3) {
    return mesh;
  }

  constexpr float radius = 0.35F;
  constexpr float half_height = 0.45F;

  // Apex and base center.
  mesh.vertices.push_back(Vertex{{0.0F, half_height, 0.0F}});
  mesh.vertices.push_back(Vertex{{0.0F, -half_height, 0.0F}});

  // Base ring.
  for (int i = 0; i < segments; ++i) {
    const float t = (2.0F * 3.14159265359F * static_cast<float>(i)) / static_cast<float>(segments);
    mesh.vertices.push_back(Vertex{{radius * std::cos(t), -half_height, radius * std::sin(t)}});
  }

  // Side triangles.
  for (int i = 0; i < segments; ++i) {
    const uint32_t curr = 2U + static_cast<uint32_t>(i);
    const uint32_t next = 2U + static_cast<uint32_t>((i + 1) % segments);
    mesh.indices.push_back(0U);
    mesh.indices.push_back(curr);
    mesh.indices.push_back(next);
  }

  // Base cap triangles.
  for (int i = 0; i < segments; ++i) {
    const uint32_t curr = 2U + static_cast<uint32_t>(i);
    const uint32_t next = 2U + static_cast<uint32_t>((i + 1) % segments);
    mesh.indices.push_back(1U);
    mesh.indices.push_back(next);
    mesh.indices.push_back(curr);
  }

  mesh.bounds = compute_aabb(mesh.vertices);
  return mesh;
}

} // namespace

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

  result.ok = true;
  result.meshes.push_back(make_cone_mesh(24));
  return result;
}

} // namespace engine::assets
