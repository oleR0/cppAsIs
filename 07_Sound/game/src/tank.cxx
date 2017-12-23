#include "tank.hxx"
#include "engine.hxx"
#include <cmath>

namespace game {

tank::tank(const float &sc)
    : quad(sc), dir(direction::up), sound_idle("na_meste.wav"),
      sound_move("ezda.wav"), sound_rotate("povorot.wav") {
  sound_idle.play_always();
}

const std::pair<vec2, float> tank::move(direction d) {
  float x = 0, y = 0, angle = 0;

  switch (d) {
  case (direction::up):
    if (dir == d)
      y = 2 * scale;
    angle = 0;
    break;
  case (direction::down):
    if (dir == d)
      y = -2 * scale;
    angle = M_PI;
    break;
  case (direction::left):
    if (dir == d)
      x = -2 * scale;
    angle = -M_PI_2;
    break;
  case (direction::right):
    if (dir == d)
      x = 2 * scale;
    angle = M_PI_2;
    break;
  }

  /*switch (d) {
  case (direction::up):
    if (position.y < 2 - 2 * scale && dir == d)
      position.y += 2 * scale;

    angle = 0;
    break;
  case (direction::down):
    if (position.y >= 2 * scale && dir == d)
      position.y -= 2 * scale;
    angle = M_PI;
    break;
  case (direction::left):
    if (position.x >= 2 * scale && dir == d)
      position.x -= 2 * scale;
    angle = -M_PI_2;
    break;
  case (direction::right):
    if (position.x < 2 - 2 * scale && dir == d)
      position.x += 2 * scale;
    angle = M_PI_2;
    break;
  }
  dir = d;
*/
  if (d == dir)
    sound_move.play();
  else
    sound_rotate.play();

  dir = d;

  return change_pos(x, y, angle);
}

} // namespace game
