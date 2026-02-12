#pragma once

#include "engine/math/mat4.h"
#include "engine/runtime/camera.h"
#include "engine/runtime/transform.h"

#include <cstdint>

namespace engine::renderer {

struct NativeWindowData {
  void* nwh = nullptr;
  void* ndt = nullptr;
  void* sdl_window = nullptr;
};

class BasicRenderer {
public:
  bool init(int width, int height, NativeWindowData native_window_data);
  void shutdown();

  void resize(int width, int height);

  void begin_frame(uint32_t clear_color_rgba = 0x1e1e28ffU);
  void submit_mesh(const runtime::Transform& transform, const runtime::Camera& camera);
  void end_frame();

  bool enabled() const;
  bool using_bgfx_backend() const;
  uint32_t draw_calls() const;

private:
  int width_ = 1;
  int height_ = 1;
  bool enabled_ = false;
  bool using_bgfx_ = false;
  void* sdl_renderer_ = nullptr;
  uint32_t draw_calls_ = 0;
};

} // namespace engine::renderer
