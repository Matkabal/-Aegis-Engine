#include "engine/core/logger.h"
#include "engine/engine.h"
#include "engine/input/input_state.h"
#include "engine/renderer/basic_renderer.h"
#include "engine/runtime/camera.h"
#include "engine/runtime/transform.h"
#include "engine/time/frame_timer.h"

#include "SDL.h"
#include "SDL_syswm.h"

#ifdef ENGINE_HAS_BGFX
#include <bgfx/bgfx.h>
#endif

#include <cstdint>
#include <string>

namespace {

engine::input::Key map_sdl_key(const SDL_Keycode keycode) {
  switch (keycode) {
  case SDLK_ESCAPE:
    return engine::input::Key::Escape;
  case SDLK_F1:
    return engine::input::Key::F1;
  case SDLK_w:
    return engine::input::Key::W;
  case SDLK_a:
    return engine::input::Key::A;
  case SDLK_s:
    return engine::input::Key::S;
  case SDLK_d:
    return engine::input::Key::D;
  case SDLK_LSHIFT:
    return engine::input::Key::LeftShift;
  case SDLK_RSHIFT:
    return engine::input::Key::RightShift;
  case SDLK_LEFT:
    return engine::input::Key::Left;
  case SDLK_RIGHT:
    return engine::input::Key::Right;
  case SDLK_UP:
    return engine::input::Key::Up;
  case SDLK_DOWN:
    return engine::input::Key::Down;
  default:
    return engine::input::Key::Unknown;
  }
}

engine::renderer::NativeWindowData query_native_window_data(SDL_Window* window, engine::core::Logger& logger) {
  engine::renderer::NativeWindowData data{};
  data.sdl_window = window;
  SDL_SysWMinfo wm_info;
  SDL_VERSION(&wm_info.version);

  if (SDL_GetWindowWMInfo(window, &wm_info) != SDL_TRUE) {
    logger.warn(std::string("SDL_GetWindowWMInfo failed: ") + SDL_GetError());
    return data;
  }

  switch (wm_info.subsystem) {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
  case SDL_SYSWM_WINDOWS:
    data.nwh = wm_info.info.win.window;
    break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
  case SDL_SYSWM_X11:
    data.nwh = reinterpret_cast<void*>(static_cast<uintptr_t>(wm_info.info.x11.window));
    data.ndt = wm_info.info.x11.display;
    break;
#endif
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
  case SDL_SYSWM_WAYLAND:
    data.nwh = wm_info.info.wl.surface;
    data.ndt = wm_info.info.wl.display;
    break;
#endif
#if defined(SDL_VIDEO_DRIVER_COCOA)
  case SDL_SYSWM_COCOA:
    data.nwh = wm_info.info.cocoa.window;
    break;
#endif
  default:
    logger.warn("Unsupported SDL WM subsystem for native renderer bridge.");
    break;
  }

  return data;
}

void draw_overlay(const engine::time::FrameMetrics& metrics,
                  const engine::runtime::Camera& camera,
                  const engine::renderer::BasicRenderer& renderer,
                  const bool show_overlay) {
  if (!show_overlay || !renderer.enabled()) {
    return;
  }

#ifdef ENGINE_HAS_BGFX
  if (!renderer.using_bgfx_backend()) {
    return;
  }
  bgfx::dbgTextClear(0, false);
  bgfx::dbgTextPrintf(0, 0, 0x0f, "M1 Sandbox (F1 overlay)");
  bgfx::dbgTextPrintf(0, 2, 0x0f, "FPS: %.2f", metrics.fps);
  bgfx::dbgTextPrintf(0, 3, 0x0f, "Frame: %.3f ms", metrics.frame_ms);
  bgfx::dbgTextPrintf(0,
                      4,
                      0x0f,
                      "Camera: x=%.2f y=%.2f z=%.2f",
                      camera.position.x,
                      camera.position.y,
                      camera.position.z);
  bgfx::dbgTextPrintf(0, 5, 0x0f, "Draw Calls: %u", renderer.draw_calls());
#else
  (void)metrics;
  (void)camera;
  (void)renderer;
#endif
}

} // namespace

int main() {
  engine::core::Logger logger;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    logger.error(std::string("SDL_Init failed: ") + SDL_GetError());
    return 1;
  }

  constexpr int initial_width = 1280;
  constexpr int initial_height = 720;

  SDL_Window* window = SDL_CreateWindow("Witcher Engine - M1",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        initial_width,
                                        initial_height,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    logger.error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    SDL_Quit();
    return 1;
  }

  int width = initial_width;
  int height = initial_height;

  engine::renderer::BasicRenderer renderer;
  const engine::renderer::NativeWindowData native_window_data = query_native_window_data(window, logger);
  const bool renderer_enabled = renderer.init(width, height, native_window_data);
  if (!renderer_enabled) {
    logger.warn("Renderer initialized in fallback mode (no backend output).");
  }

  engine::runtime::Camera camera;
  camera.set_viewport(width, height);

  engine::runtime::Transform transform;
  transform.position = {0.0F, 0.0F, 0.0F};
  transform.scale = {1.0F, 1.0F, 1.0F};
  transform.mark_dirty();

  engine::Engine engine;
  engine.initialize();

  engine::time::FrameTimer timer;
  timer.set_max_delta(0.100);
  engine::input::InputState input;

  bool running = true;
  bool show_overlay = true;

  logger.info("M1 main loop started.");

  while (running) {
    input.begin_frame();

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN: {
        if (event.key.repeat != 0) {
          break;
        }

        const engine::input::Key key = map_sdl_key(event.key.keysym.sym);
        input.on_key_down(key);

        if (key == engine::input::Key::Escape) {
          running = false;
        } else if (key == engine::input::Key::F1) {
          show_overlay = !show_overlay;
        }
        break;
      }
      case SDL_KEYUP:
        input.on_key_up(map_sdl_key(event.key.keysym.sym));
        break;
      case SDL_MOUSEMOTION:
        input.on_mouse_move(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
        break;
      case SDL_MOUSEWHEEL:
        input.on_scroll(static_cast<float>(event.wheel.preciseY));
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          width = event.window.data1;
          height = event.window.data2;
          camera.set_viewport(width, height);
          renderer.resize(width, height);
        }
        break;
      default:
        break;
      }
    }

    const engine::time::FrameMetrics metrics = timer.tick();

    camera.update(static_cast<float>(metrics.delta_seconds), input);
    bool transform_moved = false;
    const float object_speed = 1.5F * static_cast<float>(metrics.delta_seconds);
    if (input.isDown(engine::input::Key::Left)) {
      transform.position.x -= object_speed;
      transform_moved = true;
    }
    if (input.isDown(engine::input::Key::Right)) {
      transform.position.x += object_speed;
      transform_moved = true;
    }
    if (input.isDown(engine::input::Key::Up)) {
      transform.position.y += object_speed;
      transform_moved = true;
    }
    if (input.isDown(engine::input::Key::Down)) {
      transform.position.y -= object_speed;
      transform_moved = true;
    }
    if (transform_moved) {
      transform.mark_dirty();
    }
    transform.update_matrix();

    engine.update(metrics.delta_seconds);

    renderer.begin_frame();
    renderer.submit_mesh(transform, camera);
    draw_overlay(metrics, camera, renderer, show_overlay);
    renderer.end_frame();

    engine.render();

    if (!renderer.enabled()) {
      SDL_Delay(1);
    }
  }

  logger.info("M1 main loop ended.");

  engine.shutdown();
  renderer.shutdown();

  SDL_DestroyWindow(window);
  SDL_Quit();

  logger.info("M1 clean shutdown completed.");
  return 0;
}
