#include "engine/renderer/basic_renderer.h"

#include "engine/math/mat4.h"
#include "engine/math/vec4.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

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

void BasicRenderer::submit_mesh(const assets::MeshData* mesh, const math::Mat4& world_matrix, const runtime::Camera& camera) {
#ifdef ENGINE_HAS_BGFX
  if (using_bgfx_ && enabled_) {
    draw_calls_ += 1;
    return;
  }
#endif

  if (!enabled_ || sdl_renderer_ == nullptr) {
    return;
  }

  constexpr math::Vec3 fallback_vertices[3] = {
      {-0.35F, -0.30F, 0.20F},
      {0.35F, -0.25F, -0.65F},
      {0.0F, 0.40F, 0.10F},
  };
  constexpr uint32_t fallback_indices[3] = {0U, 1U, 2U};

  const math::Vec3* vertices = fallback_vertices;
  size_t vertex_count = 3;
  const uint32_t* indices = fallback_indices;
  size_t index_count = 3;
  if (mesh != nullptr && mesh->vertices.size() >= 3) {
    vertices = &mesh->vertices[0].position;
    vertex_count = mesh->vertices.size();
    if (mesh->indices.size() >= 3) {
      indices = mesh->indices.data();
      index_count = mesh->indices.size();
    }
  }

  const math::Mat4 vp = math::multiply(camera.projection, camera.view);
  const math::Mat4 mvp = math::multiply(vp, world_matrix);

  std::vector<SDL_Vertex> verts(vertex_count);
  for (size_t i = 0; i < vertex_count; ++i) {
    const math::Vec4 model{vertices[i].x, vertices[i].y, vertices[i].z, 1.0F};
    math::Vec4 clip = math::multiply_vec4(mvp, model);
    if (std::fabs(clip.w) < 0.0001F) {
      clip.w = (clip.w < 0.0F) ? -0.0001F : 0.0001F;
    }

    verts[i].position = to_screen(clip, width_, height_);

    const float depth = std::clamp((clip.z / clip.w + 1.0F) * 0.5F, 0.0F, 1.0F);
    const Uint8 shade = static_cast<Uint8>(220.0F - (depth * 100.0F));
    verts[i].color = SDL_Color{255, shade, 90, 255};
    verts[i].tex_coord = SDL_FPoint{0.0F, 0.0F};
  }

  std::vector<int> sdl_indices;
  sdl_indices.reserve(index_count);
  for (size_t i = 0; i < index_count; ++i) {
    if (static_cast<size_t>(indices[i]) >= vertex_count) {
      return;
    }
    sdl_indices.push_back(static_cast<int>(indices[i]));
  }

  SDL_Renderer* renderer = reinterpret_cast<SDL_Renderer*>(sdl_renderer_);
  SDL_RenderGeometry(renderer,
                     nullptr,
                     verts.data(),
                     static_cast<int>(verts.size()),
                     sdl_indices.data(),
                     static_cast<int>(sdl_indices.size()));

  draw_calls_ += static_cast<uint32_t>(sdl_indices.size() / 3U);
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
