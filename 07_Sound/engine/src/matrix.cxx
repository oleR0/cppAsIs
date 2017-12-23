#include "engine.hxx"
#include <cmath>
namespace tme {

mat3x2::mat3x2() : raw{vec2()} {}

const mat3x2 mat3x2::identity() { return mat3x2::scale(1.f); }

const mat3x2 mat3x2::scale(const float &scale) {
  mat3x2 result;
  result.raw[0].x = scale;
  result.raw[1].y = scale;
  return result;
}

const mat3x2 mat3x2::rotation(const float &thetha) {
  mat3x2 result;

  result.raw[0].x = std::cos(thetha);
  result.raw[1].x = std::sin(thetha);

  result.raw[0].y = -std::sin(thetha);
  result.raw[1].y = std::cos(thetha);

  return result;
}

const mat3x2 mat3x2::movement(const tme::vec2 &offset) {
  mat3x2 result = mat3x2::identity();
  result.raw[2].x = offset.x;
  result.raw[2].y = offset.y;
  return result;
}

const mat3x2 mat3x2::match_640x480() {
  mat3x2 result;
  result.raw[0].x = 1;
  result.raw[1].y = 640.f / 480.f;
  return result;
}

const std::vector<float> mat3x2::get_floats() const {
  std::vector<float> result;
  for (int i = 0; i < 3; ++i)
    result.push_back(raw[i].x);
  for (int i = 0; i < 3; ++i)
    result.push_back(raw[i].y);
  result.push_back(0);
  result.push_back(0);
  result.push_back(1);

  return result;
}
} // namespace tme
