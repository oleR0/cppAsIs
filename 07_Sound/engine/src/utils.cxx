#include "SDL2/SDL.h"
#include "engine.hxx"
#include <array>
#include <fstream>

namespace tme {

static std::array<std::string_view, 17> event_names = {
    /// input events
    {"left_pressed", "left_released", "right_pressed", "right_released",
     "up_pressed", "up_released", "down_pressed", "down_released",
     "select_pressed", "select_released", "start_pressed", "start_released",
     "button1_pressed", "button1_released", "button2_pressed",
     "button2_released",
     /// virtual console events
     "turn_off"}};

vec2 operator+(const vec2 &l, const vec2 &r) {
  vec2 result;
  result.x = l.x + r.x;
  result.y = l.y + r.y;
  return result;
}

vec2 operator*(const float &f, const vec2 &v) {
  vec2 result;
  result.x = f * v.x;
  result.y = f * v.y;
  return result;
}

vec2 operator*(const vec2 &v, const mat3x2 &m) {
  vec2 result;
  result.x = v.x * m.raw[0].x + v.y * m.raw[1].x + m.raw[2].x;
  result.y = v.x * m.raw[0].y + v.y * m.raw[1].y + m.raw[2].y;
  return result;
}

mat3x2 operator*(const mat3x2 &m1, const mat3x2 &m2) {
  mat3x2 r;

  r.raw[0].x = m1.raw[0].x * m2.raw[0].x + m1.raw[0].y * m2.raw[1].x;
  r.raw[0].y = m1.raw[0].x * m2.raw[0].y + m1.raw[0].y * m2.raw[1].y;
  r.raw[1].x = m1.raw[1].x * m2.raw[0].x + m1.raw[1].y * m2.raw[1].x;
  r.raw[1].y = m1.raw[1].x * m2.raw[0].y + m1.raw[1].y * m2.raw[1].y;
  r.raw[2].x =
      m1.raw[2].x * m2.raw[0].x + m1.raw[2].y * m2.raw[1].x + m2.raw[2].x;
  r.raw[2].y =
      m1.raw[2].x * m2.raw[0].y + m1.raw[2].y * m2.raw[1].y + m2.raw[2].y;

  return r;
}

std::ostream &operator<<(std::ostream &stream, const event e) {
  std::uint32_t value = static_cast<std::uint32_t>(e);
  std::uint32_t minimal = static_cast<std::uint32_t>(event::left_pressed);
  std::uint32_t maximal = static_cast<std::uint32_t>(event::turn_off);
  if (value >= minimal && value <= maximal) {
    stream << event_names[value];
    return stream;
  } else {
    throw std::runtime_error("too big event value");
  }
}

std::istream &operator>>(std::istream &is, mat3x2 &m) {
  is >> m.raw[0].x;
  is >> m.raw[0].y;
  is >> m.raw[1].x;
  is >> m.raw[1].y;
  return is;
}

std::istream &operator>>(std::istream &is, vec2 &v) {
  is >> v.x;
  is >> v.y;
  return is;
}

std::istream &operator>>(std::istream &is, color &c) {
  float r = 0.f;
  float g = 0.f;
  float b = 0.f;
  float a = 0.f;
  is >> r;
  is >> g;
  is >> b;
  is >> a;
  c = color(r, g, b, a);
  return is;
}

std::istream &operator>>(std::istream &is, v0 &v) {
  is >> v.pos.x;
  is >> v.pos.y;

  return is;
}

std::istream &operator>>(std::istream &is, v1 &v) {
  is >> v.pos.x;
  is >> v.pos.y;
  is >> v.c;
  return is;
}

std::istream &operator>>(std::istream &is, v2 &v) {
  is >> v.pos.x;
  is >> v.pos.y;
  is >> v.uv;
  is >> v.c;
  return is;
}

std::istream &operator>>(std::istream &is, tri0 &t) {
  is >> t.v[0];
  is >> t.v[1];
  is >> t.v[2];
  return is;
}

std::istream &operator>>(std::istream &is, tri1 &t) {
  is >> t.v[0];
  is >> t.v[1];
  is >> t.v[2];
  return is;
}

std::istream &operator>>(std::istream &is, tri2 &t) {
  is >> t.v[0];
  is >> t.v[1];
  is >> t.v[2];
  return is;
}

} // namespace tme
