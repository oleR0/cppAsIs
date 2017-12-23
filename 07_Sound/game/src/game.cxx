#include "engine.hxx"
#include "tank.hxx"
#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>
/*
tme::v0 blend(const tme::v0 &vl, const tme::v0 &vr, const float a) {
  tme::v0 r;
  r.pos.x = (1.0f - a) * vl.pos.x + a * vr.pos.x;
  r.pos.y = (1.0f - a) * vl.pos.y + a * vr.pos.y;
  return r;
}

tme::tri0 blend(const tme::tri0 &tl, const tme::tri0 &tr, const float a) {
  tme::tri0 r;
  r.v[0] = blend(tl.v[0], tr.v[0], a);
  r.v[1] = blend(tl.v[1], tr.v[1], a);
  r.v[2] = blend(tl.v[2], tr.v[2], a);
  return r;
}
*/

int main(int /*argc*/, char * /*argv*/ []) {

  std::unique_ptr<tme::engine, void (*)(tme::engine *)> engine(
      tme::create_engine(), tme::destroy_engine);

  float counter = 1;
  engine->count_to_1(&counter, 10);
  const std::string error = engine->initialize("");
  if (!error.empty()) {
    std::cerr << error << std::endl;
    return EXIT_FAILURE;
  }

  tme::texture *texture = engine->create_texture("tank.png");
  if (nullptr == texture) {
    std::cerr << "failed load texture\n";
    return EXIT_FAILURE;
  }

  std::ifstream file("scale.txt");
  assert(!!file);

  unsigned int scale;
  file >> scale;
  float antiscale = 1.0f / static_cast<float>(scale);

  game::tank q(antiscale);

  tme::mat3x2 m_movement =
      tme::mat3x2::movement(tme::vec2(antiscale - 1, antiscale - 1));
  tme::mat3x2 m_rotation = tme::mat3x2::rotation(0);
  std::pair<tme::vec2, float> move;

  bool continue_loop = true;

  while (continue_loop) {
    tme::event event;

    while (engine->read_input(event)) {

      if (counter < 1)
        continue;

      std::cout << event << std::endl;

      switch (event) {

      case tme::event::turn_off:
        continue_loop = false;
        break;

      case tme::event::up_pressed:
        counter = 0;
        m_movement = m_movement * tme::mat3x2::movement(move.first);
        m_rotation = m_rotation * tme::mat3x2::rotation(move.second);
        move = q.move(game::tank::direction::up);
        break;
      case tme::event::down_pressed:
        counter = 0;
        m_movement = m_movement * tme::mat3x2::movement(move.first);
        m_rotation = m_rotation * tme::mat3x2::rotation(move.second);
        move = q.move(game::tank::direction::down);
        break;
      case tme::event::left_pressed:
        counter = 0;
        m_movement = m_movement * tme::mat3x2::movement(move.first);
        m_rotation = m_rotation * tme::mat3x2::rotation(move.second);
        move = q.move(game::tank::direction::left);
        break;
      case tme::event::right_pressed:
        counter = 0;
        m_movement = m_movement * tme::mat3x2::movement(move.first);
        m_rotation = m_rotation * tme::mat3x2::rotation(move.second);
        move = q.move(game::tank::direction::right);
        break;
      default:
        break;
      }
    }

    (q.get_angle());
    tme::tri2 *quad_triangles = q.get_tri2s();
    engine->render(quad_triangles[0], texture,
                   m_rotation * tme::mat3x2::rotation(counter * move.second),
                   m_movement * tme::mat3x2::movement(counter * move.first));
    engine->render(quad_triangles[1], texture,
                   m_rotation * tme::mat3x2::rotation(counter * move.second),
                   m_movement * tme::mat3x2::movement(counter * move.first));

    engine->swap_buffers();
  }

  engine->uninitialize();

  return EXIT_SUCCESS;
}
