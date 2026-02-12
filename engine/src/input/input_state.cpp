#include "engine/input/input_state.h"

namespace engine::input {

void InputState::begin_frame() {
  pressed_.clear();
  released_.clear();
  mouse_delta_ = {};
  scroll_delta_ = 0.0F;
}

void InputState::on_key_down(Key key) {
  if (key == Key::Unknown) {
    return;
  }

  if (!down_.contains(key)) {
    pressed_.insert(key);
  }
  down_.insert(key);
}

void InputState::on_key_up(Key key) {
  if (key == Key::Unknown) {
    return;
  }

  if (down_.contains(key)) {
    released_.insert(key);
  }
  down_.erase(key);
}

void InputState::on_mouse_move(float x, float y) {
  mouse_delta_.x += (x - mouse_position_.x);
  mouse_delta_.y += (y - mouse_position_.y);
  mouse_position_.x = x;
  mouse_position_.y = y;
}

void InputState::on_scroll(float delta_y) {
  scroll_delta_ += delta_y;
}

bool InputState::is_down(Key key) const {
  return down_.contains(key);
}

bool InputState::was_pressed(Key key) const {
  return pressed_.contains(key);
}

bool InputState::was_released(Key key) const {
  return released_.contains(key);
}

bool InputState::isDown(Key key) const {
  return is_down(key);
}

bool InputState::wasPressed(Key key) const {
  return was_pressed(key);
}

bool InputState::wasReleased(Key key) const {
  return was_released(key);
}

math::Vec2 InputState::mouse_position() const {
  return mouse_position_;
}

math::Vec2 InputState::mouse_delta() const {
  return mouse_delta_;
}

float InputState::scroll_delta() const {
  return scroll_delta_;
}

math::Vec2 InputState::mousePosition() const {
  return mouse_position();
}

math::Vec2 InputState::mouseDelta() const {
  return mouse_delta();
}

float InputState::scrollDelta() const {
  return scroll_delta();
}

} // namespace engine::input
