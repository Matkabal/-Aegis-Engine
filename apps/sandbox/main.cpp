#include "engine/core/logger.h"
#include "engine/engine.h"
#include "engine/input/input_state.h"
#include "engine/time/frame_timer.h"

#include "SDL.h"
#include "SDL_syswm.h"

#ifdef ENGINE_HAS_BGFX
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

engine::input::Key map_sdl_key(const SDL_Keycode keycode) {
  switch (keycode) {
  case SDLK_ESCAPE:
    return engine::input::Key::Escape;
  case SDLK_F1:
    return engine::input::Key::F1;
  default:
    return engine::input::Key::Unknown;
  }
}

#ifdef ENGINE_HAS_BGFX
const char* subsystem_name(SDL_SYSWM_TYPE subsystem) {
  switch (subsystem) {
  case SDL_SYSWM_WINDOWS:
    return "WINDOWS";
  case SDL_SYSWM_X11:
    return "X11";
  case SDL_SYSWM_WAYLAND:
    return "WAYLAND";
  case SDL_SYSWM_COCOA:
    return "COCOA";
  default:
    return "UNKNOWN";
  }
}

bool get_native_handles(SDL_Window* window, void** out_nwh, void** out_ndt, std::string* error) {
  SDL_SysWMinfo wm_info;
  SDL_VERSION(&wm_info.version);
  if (SDL_GetWindowWMInfo(window, &wm_info) != SDL_TRUE) {
    *error = std::string("SDL_GetWindowWMInfo failed: ") + SDL_GetError();
    return false;
  }

  *out_nwh = nullptr;
  *out_ndt = nullptr;

  switch (wm_info.subsystem) {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
  case SDL_SYSWM_WINDOWS:
    *out_nwh = wm_info.info.win.window;
    break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
  case SDL_SYSWM_X11:
    *out_nwh = reinterpret_cast<void*>(static_cast<uintptr_t>(wm_info.info.x11.window));
    *out_ndt = wm_info.info.x11.display;
    break;
#endif
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
  case SDL_SYSWM_WAYLAND:
    *out_nwh = wm_info.info.wl.surface;
    *out_ndt = wm_info.info.wl.display;
    break;
#endif
#if defined(SDL_VIDEO_DRIVER_COCOA)
  case SDL_SYSWM_COCOA:
    *out_nwh = wm_info.info.cocoa.window;
    break;
#endif
  default:
    *error = std::string("Unsupported SDL subsystem for bgfx: ") + subsystem_name(wm_info.subsystem);
    return false;
  }

  if (*out_nwh == nullptr) {
    *error = "Native window handle is null.";
    return false;
  }

  return true;
}

bgfx::RendererType::Enum renderer_from_env() {
  const char* value = std::getenv("BGFX_RENDERER_TYPE");
  if (value == nullptr) {
    return bgfx::RendererType::Count;
  }

  const std::string renderer(value);
  if (renderer == "opengl") {
    return bgfx::RendererType::OpenGL;
  }
  if (renderer == "gles" || renderer == "opengles") {
    return bgfx::RendererType::OpenGLES;
  }
  if (renderer == "vulkan") {
    return bgfx::RendererType::Vulkan;
  }
  if (renderer == "noop") {
    return bgfx::RendererType::Noop;
  }

  return bgfx::RendererType::Count;
}

bool init_bgfx(SDL_Window* window, int width, int height, engine::core::Logger& logger) {
  void* nwh = nullptr;
  void* ndt = nullptr;
  std::string error;
  if (!get_native_handles(window, &nwh, &ndt, &error)) {
    if (error.find("operation is not supported") != std::string::npos) {
      logger.warn(error);
    } else {
      logger.error(error);
    }
    return false;
  }

  bgfx::Init init{};
  init.type = renderer_from_env();
  init.platformData.nwh = nwh;
  init.platformData.ndt = ndt;
  init.resolution.width = static_cast<uint32_t>(width);
  init.resolution.height = static_cast<uint32_t>(height);
  init.resolution.reset = BGFX_RESET_VSYNC;

  if (!bgfx::init(init)) {
    logger.error("bgfx::init failed.");
    return false;
  }

  bgfx::setViewClear(0,
                     BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     0x1e1e28ff,
                     1.0f,
                     0);

  logger.info("bgfx initialized successfully.");
  return true;
}
#endif

void draw_stats_overlay(const engine::time::FrameMetrics& metrics,
                        int width,
                        int height,
                        const engine::input::InputState& input) {
#ifdef ENGINE_HAS_BGFX
  bgfx::dbgTextClear(0, false);
  bgfx::dbgTextPrintf(0, 0, 0x0f, "M0 Sandbox Stats (F1 to toggle)");
  bgfx::dbgTextPrintf(0, 2, 0x0f, "FPS: %.2f", metrics.fps);
  bgfx::dbgTextPrintf(0, 3, 0x0f, "Frame: %.3f ms", metrics.frame_ms);
  bgfx::dbgTextPrintf(0, 4, 0x0f, "Delta: %.6f s", metrics.delta_seconds);
  bgfx::dbgTextPrintf(0, 5, 0x0f, "Resolution: %dx%d", width, height);

  const engine::input::Vec2 mouse_pos = input.mouse_position();
  const engine::input::Vec2 mouse_delta = input.mouse_delta();
  bgfx::dbgTextPrintf(0,
                      6,
                      0x0f,
                      "Mouse: (%.1f, %.1f) d(%.1f, %.1f) scroll=%.1f",
                      mouse_pos.x,
                      mouse_pos.y,
                      mouse_delta.x,
                      mouse_delta.y,
                      input.scroll_delta());
#else
  (void)metrics;
  (void)width;
  (void)height;
  (void)input;
#endif
}

} // namespace

int main() {
  engine::core::Logger logger;

  const int video_driver_count = SDL_GetNumVideoDrivers();
  std::string video_drivers = "SDL video drivers available:";
  for (int i = 0; i < video_driver_count; ++i) {
    const char* name = SDL_GetVideoDriver(i);
    video_drivers += ' ';
    video_drivers += (name != nullptr ? name : "<null>");
  }
  logger.info(video_drivers);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    logger.error(std::string("SDL_Init failed: ") + SDL_GetError());
    return 1;
  }
  logger.info(std::string("SDL current video driver: ") +
              (SDL_GetCurrentVideoDriver() != nullptr ? SDL_GetCurrentVideoDriver() : "<null>"));

  constexpr int initial_width = 1280;
  constexpr int initial_height = 720;

  SDL_Window* window = SDL_CreateWindow("Witcher Engine - M0",
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

  bool bgfx_enabled = false;
#ifdef ENGINE_HAS_BGFX
  bgfx_enabled = init_bgfx(window, width, height, logger);
  if (!bgfx_enabled) {
    logger.warn("Falling back to no-renderer mode for this run.");
  }
#else
  logger.warn("Running without bgfx. Install bgfx and configure CMake with vcpkg toolchain to enable renderer.");
#endif

  engine::Engine engine;
  engine.initialize();

  engine::time::FrameTimer timer;
  timer.set_max_delta(0.100);

  engine::input::InputState input;

  bool running = true;
  bool show_overlay = true;

  logger.info("Main loop started.");

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
      case SDL_KEYUP: {
        const engine::input::Key key = map_sdl_key(event.key.keysym.sym);
        input.on_key_up(key);
        break;
      }
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
#ifdef ENGINE_HAS_BGFX
          if (bgfx_enabled) {
            bgfx::reset(static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height),
                        BGFX_RESET_VSYNC);
          }
#endif
        }
        break;
      default:
        break;
      }
    }

    const engine::time::FrameMetrics metrics = timer.tick();

    engine.update(metrics.delta_seconds);
    engine.render();

#ifdef ENGINE_HAS_BGFX
    if (bgfx_enabled) {
      bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));
      bgfx::touch(0);
      if (show_overlay) {
        draw_stats_overlay(metrics, width, height, input);
      }
      bgfx::frame();
    } else {
      SDL_Delay(1);
    }
#else
    SDL_Delay(1);
#endif
  }

  logger.info("Main loop ended.");

  engine.shutdown();

#ifdef ENGINE_HAS_BGFX
  if (bgfx_enabled) {
    bgfx::shutdown();
  }
#endif
  SDL_DestroyWindow(window);
  SDL_Quit();

  logger.info("Clean shutdown completed.");
  return 0;
}
