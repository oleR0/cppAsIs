#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
#include <cassert>
#include <iostream>

namespace tme {

#define GL_CHECK()                                                             \
  {                                                                            \
    const unsigned int err = glGetError();                                     \
    if (err != GL_NO_ERROR) {                                                  \
      switch (err) {                                                           \
      case GL_INVALID_ENUM:                                                    \
        std::cerr << "GL_INVALID_ENUM" << std::endl;                           \
        break;                                                                 \
      case GL_INVALID_VALUE:                                                   \
        std::cerr << "GL_INVALID_VALUE" << std::endl;                          \
        break;                                                                 \
      case GL_INVALID_OPERATION:                                               \
        std::cerr << "GL_INVALID_OPERATION" << std::endl;                      \
        break;                                                                 \
      case GL_INVALID_FRAMEBUFFER_OPERATION:                                   \
        std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;          \
        break;                                                                 \
      case GL_OUT_OF_MEMORY:                                                   \
        std::cerr << "GL_OUT_OF_MEMORY" << std::endl;                          \
        break;                                                                 \
      }                                                                        \
      assert(false);                                                           \
    }                                                                          \
  }
class gl {
public:
  // we have to load all extension GL function pointers
  // dynamically from OpenGL library
  // so first declare function pointers for all we need
  static PFNGLCREATESHADERPROC glCreateShader;
  static PFNGLSHADERSOURCEARBPROC glShaderSource;
  static PFNGLCOMPILESHADERARBPROC glCompileShader;
  static PFNGLGETSHADERIVPROC glGetShaderiv;
  static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
  static PFNGLDELETESHADERPROC glDeleteShader;
  static PFNGLCREATEPROGRAMPROC glCreateProgram;
  static PFNGLATTACHSHADERPROC glAttachShader;
  static PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
  static PFNGLLINKPROGRAMPROC glLinkProgram;
  static PFNGLGETPROGRAMIVPROC glGetProgramiv;
  static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
  static PFNGLDELETEPROGRAMPROC glDeleteProgram;
  static PFNGLUSEPROGRAMPROC glUseProgram;
  static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
  static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
  static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
  static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
  static PFNGLUNIFORM1IPROC glUniform1i;
  static PFNGLACTIVETEXTUREPROC glActiveTextureMY;
  static PFNGLUNIFORM4FVPROC glUniform4fv;
  static PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;

  static void init();
};
}
