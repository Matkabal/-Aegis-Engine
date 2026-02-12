#pragma once

#include "engine/assets/asset_manager.h"
#include "engine/math/mat4.h"
#include "engine/runtime/camera.h"
#include "engine/runtime/entity.h"
#include "engine/runtime/transform.h"

#include <optional>
#include <unordered_map>
#include <vector>

namespace engine::runtime {

struct MeshComponent {
  assets::MeshHandle mesh;
};

struct CameraComponent {
  bool primary = false;
};

class Scene {
public:
  Entity create_entity(std::optional<uint32_t> parent_id = std::nullopt);

  Transform& transform(Entity entity);
  const Transform* find_transform(Entity entity) const;

  void add_mesh_component(Entity entity, MeshComponent mesh);
  const MeshComponent* find_mesh_component(Entity entity) const;

  void add_camera_component(Entity entity, CameraComponent camera);
  const CameraComponent* find_camera_component(Entity entity) const;

  std::vector<Entity> entities() const;

  bool compute_world_matrix(Entity entity, math::Mat4* out_world) const;

  uint32_t entity_count() const;
  uint32_t mesh_component_count() const;

private:
  struct Node {
    Entity entity;
    std::optional<uint32_t> parent_id;
  };

  uint32_t next_entity_id_ = 1;
  std::vector<Node> nodes_;
  std::unordered_map<uint32_t, Transform> transforms_;
  std::unordered_map<uint32_t, MeshComponent> mesh_components_;
  std::unordered_map<uint32_t, CameraComponent> camera_components_;
};

} // namespace engine::runtime
