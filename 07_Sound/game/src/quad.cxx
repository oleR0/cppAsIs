#include "quad.hxx"
#include "engine.hxx"

namespace game {

quad::quad(const float &sc) : position(vec2(0, 0)), scale(sc), angle(0) {
  fill_vert_coord();
  fill_text_coord();
}

const std::pair<vec2, float> quad::change_pos(const float &x, const float &y,
                                              const float &a) {
  vec2 curr_pos = get_position();
  float curr_angle = get_angle();
  if (position.x < 2 - x && position.x >= -x && position.y < 2 - y &&
      position.y >= -y) {
    position.x += x;
    position.y += y;
  }
  angle = a;

  return std::make_pair(vec2(position.x - curr_pos.x, position.y - curr_pos.y),
                        angle - curr_angle);
}

float quad::get_angle() const { return angle; }

vec2 quad::get_position() const { return position; }
tri2 *quad::get_tri2s() const { return const_cast<tri2 *>(tr); }

void quad::fill_vert_coord() {
  tr[0].v[0].pos = vec2(-scale, scale);
  tr[0].v[1].pos = vec2(scale, -scale);
  tr[0].v[2].pos = vec2(-scale, -scale);

  tr[1].v[0].pos = vec2(-scale, scale);
  tr[1].v[1].pos = vec2(scale, scale);
  tr[1].v[2].pos = vec2(scale, -scale);
}

void quad::fill_text_coord() {
  tr[0].v[0].uv = tr[1].v[0].uv = vec2(0.0f, 0.0f);
  tr[1].v[1].uv = vec2(1.0f, 0.0f);
  tr[0].v[1].uv = tr[1].v[2].uv = vec2(1.0f, 1.0f);
  tr[0].v[2].uv = vec2(0.0f, 1.0f);
}

} // namespace game
