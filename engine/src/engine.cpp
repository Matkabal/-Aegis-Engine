#include "engine/engine.h"

#include <iostream>

namespace engine {

void Engine::initialize() {
  std::cout << "Engine initialize.\n";
}

void Engine::update(double dt_seconds) {
  (void)dt_seconds;
}

void Engine::render() {}

void Engine::shutdown() {
  std::cout << "Engine shutdown.\n";
}

void Engine::run() {
  std::cout << "Engine bootstrap running.\n";
}

} // namespace engine
