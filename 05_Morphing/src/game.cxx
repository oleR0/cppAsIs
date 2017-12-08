#include "engine.hxx"
#include <cassert>
#include <ctime>
#include <iostream>
#include <string>

TME::vertex
blend_vertex(const TME::vertex& vl, const TME::vertex& vr, const float a)
{
  TME::vertex r;
  r.x = (1.0f - a) * vl.x + a * vr.x;
  r.y = (1.0f - a) * vl.y + a * vr.y;
  return r;
}

TME::triangle
blend(const TME::triangle& tl, const TME::triangle& tr, const float a)
{
  TME::triangle r;
  r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
  r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
  r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
  return r;
}

int
main(int a, char** b)
{
  std::string init_result = TME::init("config.json");
  if (!init_result.empty())
    return EXIT_FAILURE;

  bool continue_loop = true;

  while (continue_loop) {
    TME::e_event event;
    while (TME::read_event(event)) {
      std::cout << ((event.action == TME::Action::move_start) ? "start "
                                                              : "end ")
                << event.move << std::endl;
      if (!event.move.compare("Select")) {
        continue_loop = false;
      }
    }
    std::ifstream file("vertexes.txt");
    assert(!!file);

    TME::triangle tr1q, tr2q, tr1t, tr2t;
    float alpha = 0.f;

    file >> tr1q >> tr2q >> tr1t >> tr2t >> alpha;
    static float alpha_static = alpha;

    time_t curr_time;
    time(&curr_time);
    static time_t start_time = curr_time;

    if (curr_time - start_time > 0.1 && alpha_static < 1) {
      alpha_static += 0.1;
      start_time = curr_time;
    }

    TME::triangle tr1 = blend(tr1q, tr1t, alpha_static);
    TME::triangle tr2 = blend(tr2q, tr2t, alpha_static);

    TME::render_triangle(tr1);
    TME::render_triangle(tr2);

    TME::swap_buffers();
  }
  return TME::finish();
}
