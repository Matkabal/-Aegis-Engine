#pragma once

namespace engine {

class Engine {
public:
  void initialize();
  void update(double dt_seconds);
  void render();
  void shutdown();

  void run();
};

} // namespace engine
