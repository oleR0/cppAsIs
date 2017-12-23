#include "shader.hxx"
#include "gl_init.hxx"
#include <exception>

namespace tme {

shader_gl_es20::shader_gl_es20(
    std::string_view vertex_src, std::string_view fragment_src,
    const std::vector<std::tuple<GLuint, const GLchar *>> &attributes) {
  vert_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
  frag_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
  if (vert_shader == 0 || frag_shader == 0) {
    throw std::runtime_error("can't compile shader");
  }
  program_id = link_shader_program(attributes);
  if (program_id == 0) {
    throw std::runtime_error("can't link shader");
  }
}
void shader_gl_es20::use() const {
  gl::glUseProgram(program_id);
  GL_CHECK();
}

void shader_gl_es20::set_uniform(std::string_view uniform_name,
                                 const mat3x2 m) const {
  const int location =
      gl::glGetUniformLocation(program_id, uniform_name.data());
  GL_CHECK();
  if (location == -1) {
    std::cerr << "can't get uniform location from shader\n";
    throw std::runtime_error("can't get uniform location");
  }
  std::vector<float> values = m.get_floats();
  gl::glUniformMatrix3fv(location, 1, GL_FALSE, &values[0]);
  GL_CHECK();
}
void shader_gl_es20::set_uniform(std::string_view uniform_name,
                                 texture_gl_es20 *texture) {
  assert(texture != nullptr);
  GLint location = gl::glGetUniformLocation(program_id, uniform_name.data());
  if (location == -1) {
    std::cerr << uniform_name.data();
    throw std::runtime_error("can't get uniform location");
  }

  unsigned int texture_unit = 0;
  gl::glActiveTextureMY(GL_TEXTURE0 + texture_unit);
  GL_CHECK();

  texture->bind();

  // http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
  gl::glUniform1i(location, static_cast<int>(0 + texture_unit));
  GL_CHECK();
}

void shader_gl_es20::set_uniform(std::string_view uniform_name,
                                 const color &c) {
  const int location =
      gl::glGetUniformLocation(program_id, uniform_name.data());
  GL_CHECK();
  if (location == -1) {
    std::cerr << "can't get uniform location from shader\n";
    throw std::runtime_error("can't get uniform location");
  }
  float values[4] = {c.get_r(), c.get_g(), c.get_b(), c.get_a()};
  gl::glUniform4fv(location, 1, &values[0]);
  GL_CHECK();
}

GLuint shader_gl_es20::compile_shader(GLenum shader_type,
                                      std::string_view src) {
  GLuint shader_id = gl::glCreateShader(shader_type);
  GL_CHECK();
  std::string_view vertex_shader_src = src;
  const char *source = vertex_shader_src.data();
  gl::glShaderSource(shader_id, 1, &source, nullptr);
  GL_CHECK();

  gl::glCompileShader(shader_id);
  GL_CHECK();

  GLint compiled_status = 0;
  gl::glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_status);
  GL_CHECK();
  if (compiled_status == 0) {
    GLint info_len = 0;
    gl::glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_len);
    GL_CHECK();
    std::vector<char> info_chars(static_cast<size_t>(info_len));
    gl::glGetShaderInfoLog(shader_id, info_len, NULL, info_chars.data());
    GL_CHECK();
    gl::glDeleteShader(shader_id);
    GL_CHECK();

    std::string shader_type_name =
        shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    std::cerr << "Error compiling shader(vertex)\n"
              << vertex_shader_src << "\n"
              << info_chars.data();
    return 0;
  }
  return shader_id;
}
GLuint shader_gl_es20::link_shader_program(
    const std::vector<std::tuple<GLuint, const GLchar *>> &attributes) {
  GLuint program_id_ = gl::glCreateProgram();
  GL_CHECK();
  if (0 == program_id_) {
    std::cerr << "failed to create gl program";
    throw std::runtime_error("can't link shader");
  }

  gl::glAttachShader(program_id_, vert_shader);
  GL_CHECK();
  gl::glAttachShader(program_id_, frag_shader);
  GL_CHECK();

  // bind attribute location
  for (const auto &attr : attributes) {
    GLuint loc = std::get<0>(attr);
    const GLchar *name = std::get<1>(attr);
    gl::glBindAttribLocation(program_id_, loc, name);
    GL_CHECK();
  }

  // link program after binding attribute locations
  gl::glLinkProgram(program_id_);
  GL_CHECK();
  // Check the link status
  GLint linked_status = 0;
  gl::glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
  GL_CHECK();
  if (linked_status == 0) {
    GLint infoLen = 0;
    gl::glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
    GL_CHECK();
    std::vector<char> infoLog(static_cast<size_t>(infoLen));
    gl::glGetProgramInfoLog(program_id_, infoLen, NULL, infoLog.data());
    GL_CHECK();
    std::cerr << "Error linking program:\n" << infoLog.data();
    gl::glDeleteProgram(program_id_);
    GL_CHECK();
    return 0;
  }
  return program_id_;
}

} // namespace tme
