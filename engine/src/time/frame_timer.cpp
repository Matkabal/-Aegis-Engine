#include "engine/time/frame_timer.h"

#include <algorithm>

namespace engine::time {

FrameTimer::FrameTimer() {
  start_ = clock::now();
  previous_ = start_;
}

FrameMetrics FrameTimer::tick() {
  const clock::time_point now = clock::now();

  FrameMetrics metrics{};
  if (first_tick_) {
    first_tick_ = false;
    previous_ = now;
    return metrics;
  }

  const std::chrono::duration<double> delta = now - previous_;
  previous_ = now;

  metrics.delta_seconds = std::clamp(delta.count(), 0.0, max_delta_seconds_);
  metrics.total_seconds = std::chrono::duration<double>(now - start_).count();
  metrics.frame_ms = metrics.delta_seconds * 1000.0;

  const double instant_fps = metrics.delta_seconds > 0.0 ? (1.0 / metrics.delta_seconds) : 0.0;
  if (smoothed_fps_ == 0.0) {
    smoothed_fps_ = instant_fps;
  } else {
    constexpr double alpha = 0.10;
    smoothed_fps_ = (alpha * instant_fps) + ((1.0 - alpha) * smoothed_fps_);
  }
  metrics.fps = smoothed_fps_;

  return metrics;
}

void FrameTimer::set_max_delta(double max_delta_seconds) {
  max_delta_seconds_ = std::max(0.001, max_delta_seconds);
}

} // namespace engine::time
