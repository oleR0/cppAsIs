#pragma once
#include "engine.hxx"

namespace game {

using namespace tme;

class quad {
public:
  // scale as parameter
  explicit quad(const float &);

  float get_angle() const;
  vec2 get_position() const;
  tri2 *get_tri2s() const;

protected:
  tri2 tr[2];
  vec2 position;
  float scale;
  float angle;

  void fill_vert_coord();
  void fill_text_coord();

  // returns position and angle before changing
  const std::pair<vec2, float> change_pos(const float &x, const float &y,
                                          const float &angle);
};
} // namespace game
