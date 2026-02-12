# Classes do M2

Baseado em: `docs/adr/0003-m2-scene-gltf-asset-pipeline.md`

## Matriz (M2)

| Classe/Estrutura | Status no M2 | Arquivo |
|---|---|---|
| `engine::runtime::Entity` | Criada | `engine/include/engine/runtime/entity.h` |
| `engine::runtime::Scene` | Criada | `engine/include/engine/runtime/scene.h` |
| `engine::runtime::MeshComponent` | Criada | `engine/include/engine/runtime/scene.h` |
| `engine::runtime::CameraComponent` | Criada | `engine/include/engine/runtime/scene.h` |
| `engine::assets::Vertex` | Criada | `engine/include/engine/assets/mesh_data.h` |
| `engine::assets::Aabb` | Criada | `engine/include/engine/assets/mesh_data.h` |
| `engine::assets::MeshData` | Criada | `engine/include/engine/assets/mesh_data.h` |
| `engine::assets::GltfLoader` | Criada | `engine/include/engine/assets/gltf_loader.h` |
| `engine::assets::AssetManager` | Criada | `engine/include/engine/assets/asset_manager.h` |
| `engine::renderer::BasicRenderer` | Modificada | `engine/include/engine/renderer/basic_renderer.h` |

## 1. `engine::runtime::Entity`

- Campo `id`
  - O que faz: identifica entidade de forma única dentro da `Scene`.

## 2. `engine::runtime::Scene`

- `create_entity(parent_id opcional)`
  - O que faz: cria entidade e transform padrão; suporta parent opcional.

- `transform(entity)`
  - O que faz: retorna referência para editar transform da entidade.

- `find_transform(entity)`
  - O que faz: consulta transform sem criar/forçar alteração.

- `add_mesh_component(entity, mesh)`
  - O que faz: anexa `MeshComponent` por entidade.

- `find_mesh_component(entity)`
  - O que faz: consulta mesh da entidade.

- `add_camera_component(entity, camera)` / `find_camera_component(entity)`
  - O que fazem: anexam/consultam componente de câmera.

- `entities()`
  - O que faz: retorna lista de entidades para iteração.

- `compute_world_matrix(entity, out_world)`
  - O que faz: resolve matriz mundial, incluindo parent chain (`parent * local`).
  - Atenção: método retorna `false` para entidade inválida/out pointer nulo.

- `entity_count()` / `mesh_component_count()`
  - O que fazem: métricas para debug e overlay.

## 3. `engine::assets::MeshData` e utilitários

- `compute_aabb(vertices)`
  - O que faz: calcula `min/max` dos vértices para gerar `Aabb`.
  - Atenção: em lista vazia retorna AABB default.

## 4. `engine::assets::GltfLoader`

- `static GltfLoadResult load(path)`
  - O que faz: valida arquivo glTF e produz `MeshData` intermediário.
  - Saída: `ok`, `meshes`, `error`.
  - Atenção: não chama renderer; só parsing/conversão de dados.

## 5. `engine::assets::AssetManager`

- `AssetManager(Logger* logger = nullptr)`
  - O que faz: inicia manager com logging opcional.

- `load_mesh(path)`
  - O que faz: resolve cache por path ou carrega via loader e cria handle.

- `get_mesh(handle)`
  - O que faz: retorna ponteiro const para `MeshData` do handle.

- `unload_mesh(handle)`
  - O que faz: remove mesh do storage e limpa cache relacionado.

- `mesh_count()`
  - O que faz: total de meshes em memória no manager.

## 6. `engine::renderer::BasicRenderer` (mudança M2)

- `submit_mesh(const MeshData* mesh, const Mat4& world_matrix, const Camera& camera)`
  - O que mudou: método passou a receber dados de asset/scene (não mais transform direto).
  - O que faz: renderiza com dados já preparados pelo pipeline Scene + AssetManager.

Atenção:
- Renderer não sabe path de arquivo nem parsing de glTF.
- Separação esperada do M2: `Loader -> AssetManager -> Scene -> Renderer`.
