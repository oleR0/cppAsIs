#include "engine.hxx"
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

/*tme::vertex
blend_vertex(const tme::vertex& vl, const tme::vertex& vr, const float a)
{
  tme::vertex r;
  r.x = (1.0f - a) * vl.x + a * vr.x;
  r.y = (1.0f - a) * vl.y + a * vr.y;
  return r;
}
*/
/*tme::triangle
blend(const tme::triangle& tl, const tme::triangle& tr, const float a)
{
  tme::triangle r;
  r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
  r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
  r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
  return r;
}
*/
int
main(int a, char** b)
{
  std::string init_result = tme::init("config.json");
  if (!init_result.empty())
    return EXIT_FAILURE;

  bool continue_loop = true;

  while (continue_loop) {
    tme::e_event event;
    while (tme::read_event(event)) {
      std::cout << ((event.action == tme::Action::move_start) ? "start "
                                                              : "end ")
                << event.move << std::endl;
      if (!event.move.compare("Select")) {
        continue_loop = false;
      }
    }
    std::ifstream file("vertexes.txt");
    assert(!!file);

    tme::triangle tr1, tr2;

    file >> tr1 >> tr2;

    float t = tme::get_time();
    float c = std::cos(t);
    float s = std::sin(t);

    for (auto& v : tr1.v) {
      v.tx += c;
      v.ty += s;
    }

    for (auto& v : tr2.v) {
      v.tx += c;
      v.ty += s;
    }

    tme::render_triangle(tr1);
    tme::render_triangle(tr2);

    tme::swap_buffers();
  }
  return tme::finish();
}
