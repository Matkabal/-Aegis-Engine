#include "engine/renderer/basic_renderer.h"

#include "engine/math/mat4.h"
#include "engine/math/vec4.h"

#include <algorithm>
#include <cstdlib>
#include <string>

#include "SDL.h"

#ifdef ENGINE_HAS_BGFX
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#endif

namespace engine::renderer {

namespace {

SDL_FPoint to_screen(const math::Vec4& clip, const int width, const int height) {
  const float inv_w = 1.0F / clip.w;
  const float ndc_x = clip.x * inv_w;
  const float ndc_y = clip.y * inv_w;

  SDL_FPoint out{};
  out.x = (ndc_x * 0.5F + 0.5F) * static_cast<float>(width);
  out.y = (1.0F - (ndc_y * 0.5F + 0.5F)) * static_cast<float>(height);
  return out;
}

} // namespace

bool BasicRenderer::init(const int width, const int height, const NativeWindowData native_window_data) {
  width_ = std::max(1, width);
  height_ = std::max(1, height);
  enabled_ = false;
  using_bgfx_ = false;
  sdl_renderer_ = nullptr;

  const char* backend_env = std::getenv("ENGINE_RENDER_BACKEND");
  const std::string backend = (backend_env != nullptr) ? std::string(backend_env) : std::string("sdl");
  const bool prefer_bgfx = (backend == "bgfx");

#ifdef ENGINE_HAS_BGFX
  if (prefer_bgfx && native_window_data.nwh != nullptr) {
    bgfx::Init init{};
    init.type = bgfx::RendererType::Count;
    init.platformData.nwh = native_window_data.nwh;
    init.platformData.ndt = native_window_data.ndt;
    init.resolution.width = static_cast<uint32_t>(width_);
    init.resolution.height = static_cast<uint32_t>(height_);
    init.resolution.reset = BGFX_RESET_VSYNC;

    if (bgfx::init(init)) {
      enabled_ = true;
      using_bgfx_ = true;
      return true;
    }
  }
#endif

  if (native_window_data.sdl_window != nullptr) {
    SDL_Window* window = reinterpret_cast<SDL_Window*>(native_window_data.sdl_window);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (renderer != nullptr) {
      sdl_renderer_ = renderer;
      enabled_ = true;
      using_bgfx_ = false;
      return true;
    }
  }

  return false;
}

void BasicRenderer::shutdown() {
#ifdef ENGINE_HAS_BGFX
  if (using_bgfx_ && enabled_) {
    bgfx::shutdown();
  }
#endif

  if (sdl_renderer_ != nullptr) {
    SDL_DestroyRenderer(reinterpret_cast<SDL_Renderer*>(sdl_renderer_));
    sdl_renderer_ = nullptr;
  }

  enabled_ = false;
  using_bgfx_ = false;
}

void BasicRenderer::resize(const int width, const int height) {
  width_ = std::max(1, width);
  height_ = std::max(1, height);

#ifdef ENGINE_HAS_BGFX
  if (using_bgfx_ && enabled_) {
    bgfx::reset(static_cast<uint32_t>(width_), static_cast<uint32_t>(height_), BGFX_RESET_VSYNC);
  }
#endif
}

void BasicRenderer::begin_frame(const uint32_t clear_color_rgba) {
  draw_calls_ = 0;

#ifdef ENGINE_HAS_BGFX
  if (using_bgfx_ && enabled_) {
    bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(width_), static_cast<uint16_t>(height_));
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clear_color_rgba, 1.0F, 0);
    bgfx::touch(0);
    return;
  }
#endif

  if (!enabled_ || sdl_renderer_ == nullptr) {
    return;
  }

  SDL_Renderer* renderer = reinterpret_cast<SDL_Renderer*>(sdl_renderer_);
  const Uint8 r = static_cast<Uint8>((clear_color_rgba >> 24) & 0xFFU);
  const Uint8 g = static_cast<Uint8>((clear_color_rgba >> 16) & 0xFFU);
  const Uint8 b = static_cast<Uint8>((clear_color_rgba >> 8) & 0xFFU);
  const Uint8 a = static_cast<Uint8>(clear_color_rgba & 0xFFU);

  SDL_SetRenderDrawColor(renderer, r, g, b, a);
  SDL_RenderClear(renderer);

  const Uint32 ticks = SDL_GetTicks();
  const Uint8 pulse = static_cast<Uint8>(96 + (ticks / 8U) % 160U);
  SDL_SetRenderDrawColor(renderer, pulse, 80, 220, 255);
  SDL_FRect center_rect{
      (static_cast<float>(width_) * 0.5F) - 20.0F,
      (static_cast<float>(height_) * 0.5F) - 20.0F,
      40.0F,
      40.0F,
  };
  SDL_RenderFillRectF(renderer, &center_rect);
}

void BasicRenderer::submit_mesh(const runtime::Transform& transform, const runtime::Camera& camera) {
#ifdef ENGINE_HAS_BGFX
  if (using_bgfx_ && enabled_) {
    draw_calls_ += 1;
    return;
  }
#endif

  if (!enabled_ || sdl_renderer_ == nullptr) {
    return;
  }

  (void)camera;

  // Robust 2D fallback path: keep triangle visible and movable in SDL renderer mode.
  constexpr math::Vec3 local_vertices[3] = {
      {-0.12F, -0.12F, 0.0F},
      {0.12F, -0.12F, 0.0F},
      {0.0F, 0.14F, 0.0F},
  };

  SDL_Vertex verts[3]{};
  for (int i = 0; i < 3; ++i) {
    const float x = local_vertices[i].x + transform.position.x;
    const float y = local_vertices[i].y + transform.position.y;
    const math::Vec4 clip{x, y, 0.0F, 1.0F};

    verts[i].position = to_screen(clip, width_, height_);
    verts[i].color = SDL_Color{255, 200, 80, 255};
    verts[i].tex_coord = SDL_FPoint{0.0F, 0.0F};
  }

  SDL_Renderer* renderer = reinterpret_cast<SDL_Renderer*>(sdl_renderer_);
  SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);

  draw_calls_ += 1;
}

void BasicRenderer::end_frame() {
#ifdef ENGINE_HAS_BGFX
  if (using_bgfx_ && enabled_) {
    bgfx::frame();
    return;
  }
#endif

  if (!enabled_ || sdl_renderer_ == nullptr) {
    return;
  }

  SDL_RenderPresent(reinterpret_cast<SDL_Renderer*>(sdl_renderer_));
}

bool BasicRenderer::enabled() const {
  return enabled_;
}

bool BasicRenderer::using_bgfx_backend() const {
  return using_bgfx_;
}

uint32_t BasicRenderer::draw_calls() const {
  return draw_calls_;
}

} // namespace engine::renderer
