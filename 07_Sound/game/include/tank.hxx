#pragma once
#include "engine.hxx"
#include "quad.hxx"

namespace game {

using namespace tme;

class tank final : public quad {
public:
  enum class direction { up, down, left, right };
  explicit tank(const float &);
  const std::pair<vec2, float> move(direction d);

private:
  direction dir;
  sound sound_idle;
  sound sound_move;
  sound sound_rotate;
};

} // namespace game
