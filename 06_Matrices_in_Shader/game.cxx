#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "engine.hxx"

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

int main(int /*argc*/, char * /*argv*/ []) {
  std::unique_ptr<tme::engine, void (*)(tme::engine *)> engine(
      tme::create_engine(), tme::destroy_engine);

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

  std::ifstream file("vert_tex_color.txt");
  assert(!!file);

  tme::quad q;
  unsigned int scale;
  file >> scale;
  float antiscale = 1.0f / static_cast<float>(scale);

  tme::mat3x2 m_initial =
      tme::mat3x2::scale(antiscale) *
      tme::mat3x2::movement(tme::vec2(-antiscale / 2.0f, -antiscale / 2.0f));
  tme::mat3x2 m_to_zero = tme::mat3x2::movement(
      tme::vec2(antiscale / 2.0f - 1, antiscale / 2.0f - 1));
  tme::mat3x2 m_movement = tme::mat3x2::movement(tme::vec2(0.0, 0.0));
  tme::mat3x2 m_rotation = tme::mat3x2::rotation(0.0);

  bool continue_loop = true;

  while (continue_loop) {
    tme::event event;

    while (engine->read_input(event)) {
      std::cout << event << std::endl;
      switch (event) {

      case tme::event::turn_off:
        continue_loop = false;
        break;

      case tme::event::up_pressed:
        switch (q.dir) {

        case tme::quad::direction::up:
          m_movement = m_movement * tme::mat3x2::movement(tme::vec2(
                                        0.0, 1.0f / static_cast<float>(scale)));
          break;

        case tme::quad::direction::down:
          m_rotation = m_rotation * tme::mat3x2::rotation(-M_PI);
          break;
        case tme::quad::direction::left:
          m_rotation = m_rotation * tme::mat3x2::rotation(M_PI_2);
          break;
        case tme::quad::direction::right:
          m_rotation = m_rotation * tme::mat3x2::rotation(-M_PI_2);
          break;
        }
        q.dir = tme::quad::direction::up;
        break;
      case tme::event::down_pressed:
        switch (q.dir) {

        case tme::quad::direction::up:
          m_rotation = m_rotation * tme::mat3x2::rotation(-M_PI);
          break;

        case tme::quad::direction::down:
          m_movement = m_movement *
                       tme::mat3x2::movement(
                           tme::vec2(0.0, -1.0f / static_cast<float>(scale)));
          break;
        case tme::quad::direction::left:
          m_rotation = m_rotation * tme::mat3x2::rotation(-M_PI_2);
          break;
        case tme::quad::direction::right:
          m_rotation = m_rotation * tme::mat3x2::rotation(M_PI_2);
          break;
        }
        q.dir = tme::quad::direction::down;
        break;
      case tme::event::left_pressed:
        switch (q.dir) {

        case tme::quad::direction::up:
          m_rotation = m_rotation * tme::mat3x2::rotation(-M_PI_2);
          break;

        case tme::quad::direction::down:
          m_rotation = m_rotation * tme::mat3x2::rotation(M_PI_2);

          break;
        case tme::quad::direction::left:
          m_movement = m_movement *
                       tme::mat3x2::movement(
                           tme::vec2(-1.0f / static_cast<float>(scale), 0.0));
          break;
        case tme::quad::direction::right:
          m_rotation = m_rotation * tme::mat3x2::rotation(M_PI);
          break;
        }
        q.dir = tme::quad::direction::left;
        break;
      case tme::event::right_pressed:
        switch (q.dir) {

        case tme::quad::direction::up:
          m_rotation = m_rotation * tme::mat3x2::rotation(M_PI_2);
          break;

        case tme::quad::direction::down:
          m_rotation = m_rotation * tme::mat3x2::rotation(-M_PI_2);

          break;
        case tme::quad::direction::left:
          m_rotation = m_rotation * tme::mat3x2::rotation(M_PI);
          break;
        case tme::quad::direction::right:
          m_movement = m_movement * tme::mat3x2::movement(tme::vec2(
                                        1.0f / static_cast<float>(scale), 0.0));
          break;
        }
        q.dir = tme::quad::direction::right;
        break;
      default:

        break;
      }
    }

    engine->render(q.tr[0], texture,
                   m_initial * m_rotation * m_to_zero * m_movement);
    engine->render(q.tr[1], texture,
                   m_initial * m_rotation * m_to_zero * m_movement);

    engine->swap_buffers();
  }

  engine->uninitialize();

  return EXIT_SUCCESS;
}
