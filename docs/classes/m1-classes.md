# Classes do M1

Baseado em: `docs/adr/0002-m1-render-camera-transform.md`

## Matriz (M1)

| Classe/Estrutura | Status no M1 | Arquivo |
|---|---|---|
| `engine::math::Vec2` | Criada | `engine/include/engine/math/vec2.h` |
| `engine::math::Vec3` | Criada | `engine/include/engine/math/vec3.h` |
| `engine::math::Vec4` | Criada | `engine/include/engine/math/vec4.h` |
| `engine::math::Quaternion` | Criada | `engine/include/engine/math/quaternion.h` |
| `engine::math::Mat4` | Criada | `engine/include/engine/math/mat4.h` |
| `engine::runtime::Transform` | Criada | `engine/include/engine/runtime/transform.h` |
| `engine::runtime::Camera` | Criada | `engine/include/engine/runtime/camera.h` |
| `engine::renderer::BasicRenderer` | Criada | `engine/include/engine/renderer/basic_renderer.h` |
| `engine::input::InputState` | Modificada | `engine/include/engine/input/input_state.h` |

## 1. Matemática (`Vec*`, `Mat4`, `Quaternion`)

`Vec3` funções:
- `operator+`, `operator-`, `operator*`
  - O que fazem: operações vetoriais básicas.
- `dot(a, b)`
  - O que faz: produto escalar (ângulo/projeção).
- `cross(a, b)`
  - O que faz: vetor perpendicular (base de orientação).
- `length(v)`, `normalize(v)`
  - O que fazem: magnitude e normalização.

`Mat4` funções:
- `identity()`
  - O que faz: retorna matriz identidade.
- `multiply(a, b)`
  - O que faz: multiplica duas matrizes 4x4.
- `multiply_point(m, v)` / `multiply_vec4(m, v)`
  - O que fazem: aplicam transformação de matriz em vetor.
- `translation`, `scale`, `rotation_x/y/z`, `rotation_euler_xyz`
  - O que fazem: constroem matrizes de transformação.
- `trs(t, r, s)`
  - O que faz: compõe Translate * Rotate * Scale.
- `look_at(eye, target, up)`
  - O que faz: constrói view matrix.
- `perspective(fov, aspect, near, far)`
  - O que faz: constrói projection matrix.

Atenção geral:
- Ordem de multiplicação importa.
- Mudança de convenção (row/column major) exige revisão global.

## 2. `engine::runtime::Transform`

- `mark_dirty()`
  - O que faz: marca `world_matrix` para recálculo.

- `update_matrix()`
  - O que faz: recalcula `world_matrix` apenas quando dirty.
  - Atenção: após editar position/rotation/scale, chamar `mark_dirty()`.

## 3. `engine::runtime::Camera`

- `update(float dt_seconds, const InputState& input)`
  - O que faz: aplica fly camera (WASD/mouse/shift), atualiza view e projection quando necessário.
  - Atenção: movimento é multiplicado por `dt_seconds`.

- `set_viewport(int width, int height)`
  - O que faz: atualiza viewport e marca projection dirty.
  - Atenção: chamar no resize da janela.

- `update_orientation(input)` (privado)
  - O que faz: atualiza yaw/pitch e vetores forward/right/up.

- `update_view()` / `update_projection()` (privados)
  - O que fazem: recomputam matrizes da câmera.

## 4. `engine::renderer::BasicRenderer`

- `init(width, height, native_window_data)`
  - O que faz: inicializa backend (`bgfx` ou fallback SDL) e estado interno.

- `shutdown()`
  - O que faz: encerra backend ativo e libera recursos.

- `resize(width, height)`
  - O que faz: atualiza viewport/swapchain.

- `begin_frame(clear_color)`
  - O que faz: inicia frame e limpa tela.

- `submit_mesh(mesh, world_matrix, camera)`
  - O que faz: submete malha para draw no backend ativo.

- `end_frame()`
  - O que faz: apresenta frame na tela.

- `enabled()`
  - O que faz: informa se há backend ativo para render.

- `using_bgfx_backend()`
  - O que faz: informa se backend atual é bgfx.

- `draw_calls()`
  - O que faz: retorna draw calls do frame atual.

Atenção:
- Ordem correta por frame: `begin_frame -> submit_mesh* -> end_frame`.

## 5. `engine::input::InputState` (mudança M1)

- Novas teclas adicionadas ao enum `Key`:
  - `W`, `A`, `S`, `D`, `LeftShift`, `RightShift`, `Left`, `Right`, `Up`, `Down`.
