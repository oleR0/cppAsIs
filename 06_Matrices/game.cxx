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

  bool continue_loop = true;
  int current_shader = 0;
  while (continue_loop) {
    tme::event event;

    while (engine->read_input(event)) {
      std::cout << event << std::endl;
      switch (event) {
      case tme::event::turn_off:
        continue_loop = false;
        break;
      case tme::event::button1_released:
        ++current_shader;
        if (current_shader > 2) {
          current_shader = 0;
        }
        break;
      default:

        break;
      }
    }

    if (current_shader == 0) {
      std::ifstream file("vert_pos.txt");
      assert(!!file);

      tme::tri0 tr1;
      tme::tri0 tr2;
      tme::tri0 tr11;
      tme::tri0 tr22;

      file >> tr1 >> tr2 >> tr11 >> tr22;

      float time = engine->get_time_from_init();
      float alpha = std::sin(time);

      tme::tri0 t1 = blend(tr1, tr11, alpha);
      tme::tri0 t2 = blend(tr2, tr22, alpha);

      engine->render(t1, tme::color(1.f, 0.f, 0.f, 1.f));
      engine->render(t2, tme::color(0.f, 1.f, 0.f, 1.f));
    }

    if (current_shader == 1) {
      std::ifstream file("vert_pos_color.txt");
      assert(!!file);

      tme::tri1 tr1;
      tme::tri1 tr2;

      file >> tr1 >> tr2;

      engine->render(tr1);
      engine->render(tr2);
    }

    if (current_shader == 2) {
      std::ifstream file("vert_tex_color.txt");
      assert(!!file);

      tme::tri2 tr1;
      tme::tri2 tr2;

      file >> tr1 >> tr2;

      float time = engine->get_time_from_init();

      tme::mat3x2 m = tme::mat3x2::reverse_png() *
                      tme::mat3x2::scale(std::abs(std::sin(time))) *
                      tme::mat3x2::rotation(time) *
                      tme::mat3x2::movement(tme::vec2(0.5, 0.2)) *
                      tme::mat3x2::match_640x480();

      for (auto &v : tr2.v) {
        v.pos = v.pos * m;
      }

      for (auto &v : tr1.v) {
        v.pos = v.pos * m;
      }
      engine->render(tr1, texture);
      engine->render(tr2, texture);
    }

    engine->swap_buffers();
  }

  engine->uninitialize();

  return EXIT_SUCCESS;
}
