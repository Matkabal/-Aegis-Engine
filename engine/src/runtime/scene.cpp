#include "engine/runtime/scene.h"

#include <algorithm>

namespace engine::runtime {

Entity Scene::create_entity(const std::optional<uint32_t> parent_id) {
  Entity e{next_entity_id_++};
  nodes_.push_back(Node{e, parent_id});
  transforms_[e.id] = Transform{};
  return e;
}

Transform& Scene::transform(const Entity entity) {
  return transforms_[entity.id];
}

const Transform* Scene::find_transform(const Entity entity) const {
  const auto it = transforms_.find(entity.id);
  if (it == transforms_.end()) {
    return nullptr;
  }
  return &it->second;
}

void Scene::add_mesh_component(const Entity entity, const MeshComponent mesh) {
  mesh_components_[entity.id] = mesh;
}

const MeshComponent* Scene::find_mesh_component(const Entity entity) const {
  const auto it = mesh_components_.find(entity.id);
  if (it == mesh_components_.end()) {
    return nullptr;
  }
  return &it->second;
}

void Scene::add_camera_component(const Entity entity, const CameraComponent camera) {
  camera_components_[entity.id] = camera;
}

const CameraComponent* Scene::find_camera_component(const Entity entity) const {
  const auto it = camera_components_.find(entity.id);
  if (it == camera_components_.end()) {
    return nullptr;
  }
  return &it->second;
}

std::vector<Entity> Scene::entities() const {
  std::vector<Entity> out;
  out.reserve(nodes_.size());
  for (const Node& node : nodes_) {
    out.push_back(node.entity);
  }
  return out;
}

bool Scene::compute_world_matrix(const Entity entity, math::Mat4* out_world) const {
  if (out_world == nullptr) {
    return false;
  }

  const auto transform_it = transforms_.find(entity.id);
  if (transform_it == transforms_.end()) {
    return false;
  }

  Transform local = transform_it->second;
  local.update_matrix();
  math::Mat4 world = local.world_matrix;

  std::optional<uint32_t> parent_id;
  for (const Node& node : nodes_) {
    if (node.entity.id == entity.id) {
      parent_id = node.parent_id;
      break;
    }
  }

  while (parent_id.has_value()) {
    const auto parent_transform_it = transforms_.find(*parent_id);
    if (parent_transform_it == transforms_.end()) {
      break;
    }

    Transform parent = parent_transform_it->second;
    parent.update_matrix();
    world = math::multiply(parent.world_matrix, world);

    std::optional<uint32_t> next_parent;
    for (const Node& node : nodes_) {
      if (node.entity.id == *parent_id) {
        next_parent = node.parent_id;
        break;
      }
    }
    parent_id = next_parent;
  }

  *out_world = world;
  return true;
}

uint32_t Scene::entity_count() const {
  return static_cast<uint32_t>(nodes_.size());
}

uint32_t Scene::mesh_component_count() const {
  return static_cast<uint32_t>(mesh_components_.size());
}

} // namespace engine::runtime
