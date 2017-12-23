#pragma once
#include "engine.hxx"

namespace tme {

class texture_gl_es20 final : public texture {
public:
  explicit texture_gl_es20(std::string_view path);
  ~texture_gl_es20() override;

  void bind() const;
  std::uint32_t get_width() const final { return width; }
  std::uint32_t get_height() const final { return height; }

private:
  std::string file_path;
  uint32_t tex_handl = 0;
  std::uint32_t width = 0;
  std::uint32_t height = 0;
};
} // namespace tme
