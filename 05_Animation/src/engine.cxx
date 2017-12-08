#include "engine.hxx"
#include "../lodepng/lodepng.h"
#include "./../nlohmann_json/json.hpp"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using json = nlohmann::json;

#if __MINGW32__
#include <SDL2/SDL_opengl.h> // on windows for mingw
#else
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
#endif

#define STRINGIFY(x) #x

#define GL_CHECK()                                                             \
  {                                                                            \
    const int err = glGetError();                                              \
    if (err != GL_NO_ERROR) {                                                  \
      switch (err) {                                                           \
        case GL_INVALID_ENUM:                                                  \
          std::cerr << GL_INVALID_ENUM << std::endl;                           \
          break;                                                               \
        case GL_INVALID_VALUE:                                                 \
          std::cerr << GL_INVALID_VALUE << std::endl;                          \
          break;                                                               \
        case GL_INVALID_OPERATION:                                             \
          std::cerr << GL_INVALID_OPERATION << std::endl;                      \
          break;                                                               \
        case GL_INVALID_FRAMEBUFFER_OPERATION:                                 \
          std::cerr << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl;          \
          break;                                                               \
        case GL_OUT_OF_MEMORY:                                                 \
          std::cerr << GL_OUT_OF_MEMORY << std::endl;                          \
          break;                                                               \
      }                                                                        \
      assert(false);                                                           \
    }                                                                          \
  }

enum class log_level
{
  log_warning,
  log_error
};

class Logger
{
public:
  static Logger& get_instance(const std::string&);
  void add_log(const std::string&, log_level);

private:
  std::ofstream log;
  Logger() = delete;
  Logger(const std::string&);
  ~Logger();
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};

Logger::Logger(const std::string& log_file)
{
  if (log_file.empty())
    log.open("Engine.log");
  else
    log.open(log_file);
}

Logger::~Logger()
{
  log.close();
}

Logger&
Logger::get_instance(const std::string& log_file)
{
  static Logger instance(log_file);
  return instance;
}

void
Logger::add_log(const std::string& log_string, log_level level)
{
  if (!log)
    return;

  std::string prefix;
  switch (level) {
    case log_level::log_warning:
      prefix = "Warning: ";
    case log_level::log_error:
      prefix = "Error: ";
  }

  log << prefix << log_string << "." << std::endl;
}

class GLtme
{
public:
  static PFNGLCREATEPROGRAMPROC glCreateProgram;
  static PFNGLCREATESHADERPROC glCreateShader;
  static PFNGLSHADERSOURCEARBPROC glShaderSource;
  static PFNGLCOMPILESHADERARBPROC glCompileShader;
  // PFNGLGETSHADERIVPROC             glGetShaderiv            ;
  // PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog       ;
  // PFNGLDELETESHADERPROC            glDeleteShader           ;
  static PFNGLATTACHSHADERPROC glAttachShader;
  static PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
  static PFNGLLINKPROGRAMPROC glLinkProgram;
  // PFNGLGETPROGRAMIVPROC            glGetProgramiv           ;
  // PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog      ;
  // PFNGLDELETEPROGRAMPROC           glDeleteProgram          ;
  static PFNGLUSEPROGRAMPROC glUseProgram;
  static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
  static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
  static PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
  static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
  static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
  static PFNGLUNIFORM1IPROC glUniform1i;
  static PFNGLACTIVETEXTUREPROC glActiveTexture;

  static GLint a_position_location;
  static GLint a_texture_coordinates_location;
  static GLint u_texture_unit_location;

  static std::string glInit()
  {
    try {
      load_gl_func(STRINGIFY(glCreateProgram), glCreateProgram);
      load_gl_func(STRINGIFY(glCreateShader), glCreateShader);
      load_gl_func(STRINGIFY(glShaderSource), glShaderSource);
      load_gl_func(STRINGIFY(glCompileShader), glCompileShader);
      load_gl_func(STRINGIFY(glAttachShader), glAttachShader);
      load_gl_func(STRINGIFY(glLinkProgram), glLinkProgram);
      load_gl_func(STRINGIFY(glUseProgram), glUseProgram);
      load_gl_func(STRINGIFY(glVertexAttribPointer), glVertexAttribPointer);
      load_gl_func(STRINGIFY(glEnableVertexAttribArray),
                   glEnableVertexAttribArray);
      load_gl_func(STRINGIFY(glBindAttribLocation), glBindAttribLocation);
      load_gl_func(STRINGIFY(glGenerateMipmap), glGenerateMipmap);
      load_gl_func(STRINGIFY(glGetAttribLocation), glGetAttribLocation);
      load_gl_func(STRINGIFY(glGetUniformLocation), glGetUniformLocation);
      load_gl_func(STRINGIFY(glUniform1i), glUniform1i);
      load_gl_func(STRINGIFY(glActiveTexture), glActiveTexture);
    } catch (std::exception& ex) {
      return ex.what();
    }
    return "";
  }

  static std::string init_program()
  {
    GLuint programObject = glCreateProgram();
    if (!programObject) {
      return "failed while creating a program";
    }

    std::string_view vertex = R"(
  attribute vec2 a_position;
  attribute vec2 a_texture_coordinates;
  varying vec2 v_texture_coordinates;
  void main()
  {
      v_texture_coordinates = a_texture_coordinates;
      gl_Position = vec4(a_position, 0.0, 1.0);
  }
  )";

    std::string_view fragment = R"(
  varying vec2 v_texture_coordinates;
  uniform sampler2D s_texture;
  void main()
  {
      gl_FragColor = texture2D(s_texture, v_texture_coordinates);
  }
  )";

    loadShader(programObject, GL_VERTEX_SHADER, vertex.data());
    GL_CHECK();
    loadShader(programObject, GL_FRAGMENT_SHADER, fragment.data());
    GL_CHECK();
    // bind attribute location
    glBindAttribLocation(programObject, 0, "a_position");
    GL_CHECK();

    glLinkProgram(programObject);
    GL_CHECK();
    glUseProgram(programObject);
    GL_CHECK();

    a_position_location = glGetAttribLocation(programObject, "a_position");
    GL_CHECK();
    a_texture_coordinates_location =
      glGetAttribLocation(programObject, "a_texture_coordinates");
    GL_CHECK();
    u_texture_unit_location =
      glGetUniformLocation(programObject, "u_texture_unit");
    GL_CHECK();

    if (init_texture("tank.png")) {
      return "failed load texture\n";
    }

    glUniform1i(u_texture_unit_location, 0);
    GL_CHECK();
    // прозрачность
    glEnable(GL_BLEND);
    GL_CHECK();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_CHECK();
    return "";
  }

private:
  GLtme() {}

  template<typename T>
  static void load_gl_func(const char* func_name, T& result)
  {
    void* gl_pointer = SDL_GL_GetProcAddress(func_name);
    if (nullptr == gl_pointer) {
      throw std::runtime_error(std::string("can't load GL function") +
                               func_name);
    }
    result = reinterpret_cast<T>(gl_pointer);
  }

  static std::string loadShader(const GLuint& program,
                                const GLenum& type,
                                const GLchar* shaderSrc)
  {
    GLuint shader;
    shader = glCreateShader(type);
    GL_CHECK();
    glShaderSource(shader, 1, &shaderSrc, NULL);
    GL_CHECK();
    glCompileShader(shader);
    GL_CHECK();
    glAttachShader(program, shader);
    GL_CHECK();
    return "";
  }

  static GLuint load_texture(const GLsizei width,
                             const GLsizei height,
                             const GLenum type,
                             const GLvoid* pixels)
  {
    GLuint texture_object_id;
    glGenTextures(1, &texture_object_id);
    assert(texture_object_id != 0);

    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    GL_CHECK();

    glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK();
    glTexImage2D(
      GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    GL_CHECK();

    glBindTexture(GL_TEXTURE_2D, 0);
    GL_CHECK();
    return texture_object_id;
  }

  static int init_texture(std::string filename)
  {
    std::vector<unsigned char> image; // the raw pixels
    unsigned width, height, error = 0;

    // decode
    error = lodepng::decode(image, width, height, filename);

    // if there's an error, display it
    if (error)
      std::cout << "decoder error " << error << ": "
                << lodepng_error_text(error) << std::endl;

    // the pixels are now in the vector "image", 4 bytes per pixel, ordered
    // RGBARGBA..., use it as texture, draw it, ...

    glActiveTexture(GL_TEXTURE0);
    GLuint texture = load_texture(width, height, GL_RGBA, &image[0]);
    if (!texture)
      return -1;

    glBindTexture(GL_TEXTURE_2D, texture);
    GL_CHECK();
    return error;
  }
};

// static members init
PFNGLCREATEPROGRAMPROC GLtme::glCreateProgram = nullptr;
PFNGLCREATESHADERPROC GLtme::glCreateShader = nullptr;
PFNGLSHADERSOURCEARBPROC GLtme::glShaderSource = nullptr;
PFNGLCOMPILESHADERARBPROC GLtme::glCompileShader = nullptr;
PFNGLATTACHSHADERPROC GLtme::glAttachShader = nullptr;
PFNGLBINDATTRIBLOCATIONPROC GLtme::glBindAttribLocation = nullptr;
PFNGLLINKPROGRAMPROC GLtme::glLinkProgram = nullptr;
PFNGLUSEPROGRAMPROC GLtme::glUseProgram = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC GLtme::glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC GLtme::glEnableVertexAttribArray = nullptr;
PFNGLGENERATEMIPMAPPROC GLtme::glGenerateMipmap = nullptr;
PFNGLGETATTRIBLOCATIONPROC GLtme::glGetAttribLocation = nullptr;
PFNGLGETUNIFORMLOCATIONPROC GLtme::glGetUniformLocation = nullptr;
PFNGLUNIFORM1IPROC GLtme::glUniform1i = nullptr;
PFNGLACTIVETEXTUREPROC GLtme::glActiveTexture = nullptr;

GLint GLtme::a_position_location;
GLint GLtme::a_texture_coordinates_location;
GLint GLtme::u_texture_unit_location;

namespace tme {
e_event&
e_event::operator=(const e_event& e)
{
  action = e.action;
  move = e.move;
}

std::istream&
operator>>(std::istream& is, vertex& v)
{
  is >> v.x;
  is >> v.y;

  is >> v.tx;
  is >> v.ty;

  return is;
}

std::istream&
operator>>(std::istream& is, triangle& t)
{
  is >> t.v[0];
  is >> t.v[1];
  is >> t.v[2];
  return is;
}

static SDL_Window* window = nullptr;

const std::string
init(const std::string& config_file)
{
  std::ifstream config_stream(config_file);
  if (!config_stream.good()) {
    std::string err("can't open config file ");
    err += config_file;
    return err;
  }

  std::string log_file("");
  try {
    json config_json;
    config_stream >> config_json;
    log_file = config_json.at("LogFile").get<std::string>();

    buttons =
      config_json.at("Buttons").get<std::map<std::string, std::string>>();
  } catch (json::exception e) {
    std::string err("exception occurred while parsing config file: ");
    return err + e.what();
  }

  Logger& logger = Logger::get_instance(log_file);

  SDL_version compiled{ 0, 0, 0 };
  SDL_version linked{ 0, 0, 0 };
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    logger.add_log("compiled SDL version differs from linked SDL version",
                   log_level::log_warning);
  }

  const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
  if (init_result != 0) {
    std::string err("failed call SDL_Init with error ");
    err += SDL_GetError();
    logger.add_log(err, log_level::log_error);
    return err;
  }

  window = SDL_CreateWindow("First SDL Window",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            640,
                            480,
                            SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    std::string err("failed call SDL_CreateWindow: ");
    err += SDL_GetError();
    logger.add_log(err, log_level::log_error);
    return err;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    std::string err("failed to initialize GL context with error ");
    err += SDL_GetError();
    logger.add_log(err, log_level::log_error);
    return err;
  }

  int gl_major_ver = 0;
  int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
  assert(result == 0);
  int gl_minor_ver = 0;
  result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
  assert(result == 0);

  if (gl_major_ver <= 2 && gl_minor_ver < 1) {
    std::string err("GL version is  ");
    err += std::to_string(gl_major_ver) + "." + std::to_string(gl_minor_ver) +
           "\nAt least 2.1 needed.";
    logger.add_log(err, log_level::log_error);
    return err;
  }
  std::string gl_result = GLtme::glInit();
  if (!gl_result.empty())
    return "Error while initializing OpenGL library";
  gl_result = GLtme::init_program();
  if (!gl_result.empty())
    return "Error while initializing program";
  return "";
}

bool
read_event(e_event& event)
{
  SDL_Event sdl_event;

  if (SDL_PollEvent(&sdl_event)) {
    if (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) {
      (sdl_event.type == SDL_KEYDOWN) ? event.action = Action::move_start
                                      : event.action = Action::move_end;

      std::string button(SDL_GetKeyName(sdl_event.key.keysym.sym));
      if (buttons.count(button)) {
        event.move = buttons[button];
        return true;
      }
    }
  }
  return false;
}

int
finish()
{
  SDL_Quit();
  return EXIT_SUCCESS;
}
void
swap_buffers()
{

  SDL_GL_SwapWindow(window);
  time_t t;
  time(&t);
  srand(t);
  double red = rand();
  red /= RAND_MAX;
  double green = rand();
  green /= RAND_MAX;
  double blue = rand();
  blue /= RAND_MAX;
  glClearColor(red, green, blue, 0.0f);
  GL_CHECK();
  glClear(GL_COLOR_BUFFER_BIT);
  GL_CHECK();
}

float
get_time()
{
  return static_cast<float>(SDL_GetTicks()) * 0.001f;
}

void
render_triangle(const triangle& t)
{

  // vertex coordinates
  GLtme::glVertexAttribPointer(GLtme::a_position_location,
                               2,
                               GL_FLOAT,
                               GL_FALSE,
                               sizeof(vertex),
                               &t.v[0].x);
  GL_CHECK();
  GLtme::glEnableVertexAttribArray(0);
  GL_CHECK();

  // texture coordinates
  GLtme::glVertexAttribPointer(GLtme::a_texture_coordinates_location,
                               2,
                               GL_FLOAT,
                               GL_FALSE,
                               sizeof(vertex),
                               &t.v[0].tx);
  GL_CHECK();
  GLtme::glEnableVertexAttribArray(1);
  GL_CHECK();

  glDrawArrays(GL_TRIANGLES, 0, 3);
  GL_CHECK();
}

} // namespace tme
