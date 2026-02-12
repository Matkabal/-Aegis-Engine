#pragma once

#include "engine/math/vec2.h"

#include <unordered_set>

namespace engine::input {

enum class Key {
  Unknown,
  Escape,
  F1,
  W,
  A,
  S,
  D,
  LeftShift,
  RightShift,
  Left,
  Right,
  Up,
  Down,
};

class InputState {
public:
  void begin_frame();

  void on_key_down(Key key);
  void on_key_up(Key key);
  void on_mouse_move(float x, float y);
  void on_scroll(float delta_y);

  bool is_down(Key key) const;
  bool was_pressed(Key key) const;
  bool was_released(Key key) const;
  bool isDown(Key key) const;
  bool wasPressed(Key key) const;
  bool wasReleased(Key key) const;

  math::Vec2 mouse_position() const;
  math::Vec2 mouse_delta() const;
  float scroll_delta() const;
  math::Vec2 mousePosition() const;
  math::Vec2 mouseDelta() const;
  float scrollDelta() const;

private:
  std::unordered_set<Key> down_;
  std::unordered_set<Key> pressed_;
  std::unordered_set<Key> released_;

  math::Vec2 mouse_position_{};
  math::Vec2 mouse_delta_{};
  float scroll_delta_ = 0.0F;
};

} // namespace engine::input
