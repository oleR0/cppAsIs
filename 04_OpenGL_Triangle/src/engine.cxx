#include "engine.hxx"
#include "./../nlohmann_json/json.hpp"
#include <SDL2/SDL.h>
#include <any>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;

#if __MINGW32__
#include <SDL2/SDL_opengl.h> // on windows for mingw
#else
#include <SDL2/SDL_opengles2.h>
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

namespace TME {
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
SDL_GLContext gl_context;

template<typename T>
static void
load_gl_func(const char* func_name, T& result)
{
  void* gl_pointer = SDL_GL_GetProcAddress(func_name);
  if (nullptr == gl_pointer) {
    throw std::runtime_error(std::string("can't load GL function") + func_name);
  }
  result = reinterpret_cast<T>(gl_pointer);
}

PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEARBPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERARBPROC glCompileShader = nullptr;
// PFNGLGETSHADERIVPROC             glGetShaderiv             = nullptr;
// PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog        = nullptr;
// PFNGLDELETESHADERPROC            glDeleteShader            = nullptr;
// PFNGLCREATEPROGRAMPROC           glCreateProgram           = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
// PFNGLBINDATTRIBLOCATIONPROC      glBindAttribLocation      = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
// PFNGLGETPROGRAMIVPROC            glGetProgramiv            = nullptr;
// PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog       = nullptr;
// PFNGLDELETEPROGRAMPROC           glDeleteProgram           = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;

void
loadShader(const GLuint& program, const GLenum& type, const GLchar* shaderSrc)
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
}

std::string
glInit()
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

  } catch (std::exception& ex) {
    return ex.what();
  }
  GLuint programObject = glCreateProgram();
  if (!programObject) {
    return "failed while creating a program";
  }

  const GLchar vertex[] = "attribute vec2 a_position;\n"
                          "void main()\n"
                          "{\n"
                          "   gl_Position = vec4(a_position, 0.0, 1.0);\n"
                          "}\0";

  const GLchar fragment[] = "void main()\n"
                            "{\n"
                            "   gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
                            "}\0";

  loadShader(programObject, GL_VERTEX_SHADER, vertex);
  loadShader(programObject, GL_FRAGMENT_SHADER, fragment);
  glLinkProgram(programObject);
  GL_CHECK();
  glUseProgram(programObject);
  GL_CHECK();

  return "";
}

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

  gl_context = SDL_GL_CreateContext(window);
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
  return glInit();
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

void
render_triangle(const triangle& t)
{
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &t.v[0]);
  GL_CHECK();
  glEnableVertexAttribArray(0);
  GL_CHECK();
  glDrawArrays(GL_TRIANGLES, 0, 3);
  GL_CHECK();
}

} // namespace TME
