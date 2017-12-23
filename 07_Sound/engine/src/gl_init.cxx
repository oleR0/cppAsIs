#include "gl_init.hxx"

namespace tme {

template <typename T>
static void load_gl_func(const char *func_name, T &result) {
  void *gl_pointer = SDL_GL_GetProcAddress(func_name);
  if (nullptr == gl_pointer) {
    throw std::runtime_error(std::string("can't load GL function") + func_name);
  }
  result = reinterpret_cast<T>(gl_pointer);
}

PFNGLCREATESHADERPROC gl::glCreateShader = nullptr;
PFNGLSHADERSOURCEARBPROC gl::glShaderSource = nullptr;
PFNGLCOMPILESHADERARBPROC gl::glCompileShader = nullptr;
PFNGLGETSHADERIVPROC gl::glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC gl::glGetShaderInfoLog = nullptr;
PFNGLDELETESHADERPROC gl::glDeleteShader = nullptr;
PFNGLCREATEPROGRAMPROC gl::glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC gl::glAttachShader = nullptr;
PFNGLBINDATTRIBLOCATIONPROC gl::glBindAttribLocation = nullptr;
PFNGLLINKPROGRAMPROC gl::glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC gl::glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC gl::glGetProgramInfoLog = nullptr;
PFNGLDELETEPROGRAMPROC gl::glDeleteProgram = nullptr;
PFNGLUSEPROGRAMPROC gl::glUseProgram = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC gl::glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC gl::glEnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC gl::glDisableVertexAttribArray = nullptr;
PFNGLGETUNIFORMLOCATIONPROC gl::glGetUniformLocation = nullptr;
PFNGLUNIFORM1IPROC gl::glUniform1i = nullptr;
PFNGLACTIVETEXTUREPROC gl::glActiveTextureMY = nullptr;
PFNGLUNIFORM4FVPROC gl::glUniform4fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC gl::glUniformMatrix3fv = nullptr;

void gl::init() {
  load_gl_func("glCreateShader", glCreateShader);
  load_gl_func("glShaderSource", glShaderSource);
  load_gl_func("glCompileShader", glCompileShader);
  load_gl_func("glGetShaderiv", glGetShaderiv);
  load_gl_func("glGetShaderInfoLog", glGetShaderInfoLog);
  load_gl_func("glDeleteShader", glDeleteShader);
  load_gl_func("glCreateProgram", glCreateProgram);
  load_gl_func("glAttachShader", glAttachShader);
  load_gl_func("glBindAttribLocation", glBindAttribLocation);
  load_gl_func("glLinkProgram", glLinkProgram);
  load_gl_func("glGetProgramiv", glGetProgramiv);
  load_gl_func("glGetProgramInfoLog", glGetProgramInfoLog);
  load_gl_func("glDeleteProgram", glDeleteProgram);
  load_gl_func("glUseProgram", glUseProgram);
  load_gl_func("glVertexAttribPointer", glVertexAttribPointer);
  load_gl_func("glEnableVertexAttribArray", glEnableVertexAttribArray);
  load_gl_func("glDisableVertexAttribArray", glDisableVertexAttribArray);
  load_gl_func("glGetUniformLocation", glGetUniformLocation);
  load_gl_func("glUniform1i", glUniform1i);
  load_gl_func("glActiveTexture", glActiveTextureMY);
  load_gl_func("glUniform4fv", glUniform4fv);
  load_gl_func("glUniformMatrix3fv", glUniformMatrix3fv);
}
} // namespace tme
