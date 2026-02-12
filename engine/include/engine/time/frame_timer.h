#pragma once

#include <chrono>

namespace engine::time {

struct FrameMetrics {
  double delta_seconds = 0.0;
  double total_seconds = 0.0;
  double frame_ms = 0.0;
  double fps = 0.0;
};

class FrameTimer {
public:
  FrameTimer();

  FrameMetrics tick();
  void set_max_delta(double max_delta_seconds);

private:
  using clock = std::chrono::steady_clock;

  clock::time_point start_;
  clock::time_point previous_;
  bool first_tick_ = true;

  double max_delta_seconds_ = 0.1;
  double smoothed_fps_ = 0.0;
};

} // namespace engine::time
