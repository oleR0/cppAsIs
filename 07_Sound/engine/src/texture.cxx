#include "texture.hxx"
#include "gl_init.hxx"
#include "lodepng.h"

namespace tme {

texture_gl_es20::texture_gl_es20(std::string_view path) : file_path(path) {

  std::vector<unsigned char> image;
  unsigned w = 0;
  unsigned h = 0;
  int error = lodepng::decode(image, w, h, file_path);

  // if there's an error, display it
  if (error != 0) {
    std::cerr << "error: " << error << std::endl;
    throw std::runtime_error("can't load texture");
  }
  //генерирует нужное количество имён для текстур
  glGenTextures(1, &tex_handl);
  GL_CHECK();
  glBindTexture(GL_TEXTURE_2D, tex_handl);
  GL_CHECK();

  GLint mipmap_level = 0;
  GLint border = 0;
  GLsizei width = static_cast<GLsizei>(w);
  GLsizei height = static_cast<GLsizei>(h);
  glTexImage2D(GL_TEXTURE_2D, mipmap_level, GL_RGBA, width, height, border,
               GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
  GL_CHECK();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  GL_CHECK();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GL_CHECK();
}

void texture_gl_es20::bind() const {
  glBindTexture(GL_TEXTURE_2D, tex_handl);
  GL_CHECK();
}

texture_gl_es20::~texture_gl_es20() {
  glDeleteTextures(1, &tex_handl);
  GL_CHECK();
}
} // namespace tme
