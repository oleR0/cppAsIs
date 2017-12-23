#include "engine_impl.hxx"
#include "gl_init.hxx"
#include <algorithm>
#include <cassert>
#include <sstream>

namespace tme {

std::ostream &operator<<(std::ostream &out, const SDL_version &v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

static bool already_exist = false;
engine *create_engine() {
  if (already_exist) {
    throw std::runtime_error("engine already exist");
  }
  engine *result = new engine_impl();
  already_exist = true;
  return result;
}

void destroy_engine(engine *e) {
  if (already_exist == false) {
    throw std::runtime_error("engine not created");
  }
  if (nullptr == e) {
    throw std::runtime_error("e is nullptr");
  }
  delete e;
}

static bool check_input(const SDL_Event &e, const bind *&result) {
  using namespace std;

  const auto it = find_if(begin(keys), end(keys), [&](const bind &b) {
    return b.key == e.key.keysym.sym;
  });

  if (it != end(keys)) {
    result = &(*it);
    return true;
  }
  return false;
}

std::string engine_impl::initialize(std::string_view) {
  using namespace std;

  stringstream serr;

  SDL_version compiled = {0, 0, 0};
  SDL_version linked = {0, 0, 0};

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    serr << "warning: SDL2 compiled and linked version mismatch: " << compiled
         << " " << linked << endl;
  }

  const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
  if (init_result != 0) {
    const char *err_message = SDL_GetError();
    serr << "error: failed call SDL_Init: " << err_message << endl;
    return serr.str();
  }

  window =
      SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       640, 480, ::SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    const char *err_message = SDL_GetError();
    serr << "error: failed call SDL_CreateWindow: " << err_message << endl;
    SDL_Quit();
    return serr.str();
  }

  gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    std::string msg("can't create opengl context: ");
    msg += SDL_GetError();
    serr << msg << endl;
    return serr.str();
  }

  int gl_major_ver = 0;
  int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
  assert(result == 0);
  int gl_minor_ver = 0;
  result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
  assert(result == 0);

  if (gl_major_ver <= 2 && gl_minor_ver < 1) {
    serr << "current context opengl version: " << gl_major_ver << '.'
         << gl_minor_ver << '\n'
         << "need opengl version at least: 2.1\n"
         << std::flush;
    return serr.str();
  }
  try {
    gl::init();
  } catch (std::exception &ex) {
    return ex.what();
  }

  shader00 = new shader_gl_es20(R"(
                                  attribute vec2 a_position;
                                  void main()
                                  {
                                  gl_Position = vec4(a_position, 0.0, 1.0);
                                  }
                                  )",
                                R"(
                                  uniform vec4 u_color;
                                  void main()
                                  {
                                  gl_FragColor = u_color;
                                  }
                                  )",
                                {{0, "a_position"}});

  shader00->use();
  shader00->set_uniform("u_color", color(1.f, 0.f, 0.f, 1.f));

  shader01 = new shader_gl_es20(
      R"(
                attribute vec2 a_position;
                attribute vec4 a_color;
                varying vec4 v_color;
                void main()
                {
                v_color = a_color;
                gl_Position = vec4(a_position, 0.0, 1.0);
                }
                )",
      R"(
                varying vec4 v_color;
                void main()
                {
                gl_FragColor = v_color;
                }
                )",
      {{0, "a_position"}, {1, "a_color"}});

  shader01->use();

  shader02 = new shader_gl_es20(
      R"(
                uniform mat3 u_matrix;
                attribute vec2 a_position;
                attribute vec2 a_tex_coord;
                attribute vec4 a_color;

                varying vec4 v_color;
                varying vec2 v_tex_coord;

                void main()
                {
                vec3 temp = vec3(a_position, 1.0);

                v_tex_coord =  a_tex_coord;
                v_color = a_color;
                temp = (temp * u_matrix);
                gl_Position = vec4(temp, 1.0);
                }
                )",
      R"(
                varying vec2 v_tex_coord;
                varying vec4 v_color;
                uniform sampler2D s_texture;


                void main()
                {
                gl_FragColor = texture2D(s_texture, v_tex_coord) * v_color;
                }
                )",
      {{0, "a_position"}, {1, "a_color"}, {2, "a_tex_coord"}});

  // turn on rendering with just created shader program
  shader02->use();

  shader_matrix = new shader_gl_es20(
      R"(
                uniform mat3 u_rotate_matrix;
                uniform mat3 u_move_matrix;
                attribute vec2 a_position;
                attribute vec2 a_tex_coord;
                attribute vec4 a_color;

                varying vec4 v_color;
                varying vec2 v_tex_coord;

                void main()
                {
                vec3 temp = vec3(a_position, 1.0);

                v_tex_coord =  a_tex_coord;
                v_color = a_color;
                temp = (temp * u_rotate_matrix * u_move_matrix);
                gl_Position = vec4(temp, 1.0);
                }
                )",
      R"(
                varying vec2 v_tex_coord;
                varying vec4 v_color;
                uniform sampler2D s_texture;


                void main()
                {
                gl_FragColor = texture2D(s_texture, v_tex_coord) * v_color;
                }
                )",
      {{0, "a_position"}, {1, "a_color"}, {2, "a_tex_coord"}});

  // turn on rendering with just created shader program
  shader_matrix->use();

  glEnable(GL_BLEND);
  GL_CHECK();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  GL_CHECK();

  glClearColor(0.f, 0.0, 0.f, 0.0f);
  GL_CHECK();

  glViewport(0, 0, 640, 480);
  GL_CHECK();

  return "";
}

uint32_t engine_impl::counter_function(uint32_t interval, void *param) {
  float *f_param = static_cast<float *>(param);
  if ((*f_param) < 1)
    (*f_param) += static_cast<float>(interval) / 1000.f;

  return interval; // repeat the timer
}

bool engine_impl::count_to_1(float *const counter, const int &interval) {
  if (!SDL_AddTimer(interval, counter_function, counter))
    return false;
  return true;
}

float engine_impl::get_time_from_init() {
  std::uint32_t ms_from_library_initialization = SDL_GetTicks();
  float seconds = ms_from_library_initialization * 0.001f;
  return seconds;
}
/// pool event from input queue
/// return true if more events in queue
bool engine_impl::read_input(event &e) {
  using namespace std;
  // collect all events from SDL
  SDL_Event sdl_event;
  if (SDL_PollEvent(&sdl_event)) {
    const bind *binding = nullptr;

    if (sdl_event.type == SDL_QUIT) {
      e = event::turn_off;
      return true;
    } else if (sdl_event.type == SDL_KEYDOWN) {
      if (check_input(sdl_event, binding)) {
        e = binding->event_pressed;
        return true;
      }
    } else if (sdl_event.type == SDL_KEYUP) {
      if (check_input(sdl_event, binding)) {
        e = binding->event_released;
        return true;
      }
    }
  }
  return false;
}

texture *engine_impl::create_texture(std::string_view path) {
  return new texture_gl_es20(path);
}
void engine_impl::destroy_texture(texture *t) { delete t; }

void engine_impl::render(const tri0 &t, const color &c) {
  shader00->use();
  shader00->set_uniform("u_color", c);
  // vertex coordinates
  gl::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v0),
                            &t.v[0].pos.x);
  GL_CHECK();
  gl::glEnableVertexAttribArray(0);
  GL_CHECK();

  // texture coordinates
  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(v0),
  // &t.v[0].tx);
  // GL_CHECK();
  // glEnableVertexAttribArray(1);
  // GL_CHECK();

  glDrawArrays(GL_TRIANGLES, 0, 3);
  GL_CHECK();
}
void engine_impl::render(const tri1 &t) {
  shader01->use();
  // positions
  gl::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                            &t.v[0].pos);
  GL_CHECK();
  gl::glEnableVertexAttribArray(0);
  GL_CHECK();
  // colors
  gl::glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                            &t.v[0].c);
  GL_CHECK();
  gl::glEnableVertexAttribArray(1);
  GL_CHECK();

  glDrawArrays(GL_TRIANGLES, 0, 3);
  GL_CHECK();

  gl::glDisableVertexAttribArray(1);
  GL_CHECK();
}
void engine_impl::render(const tri2 &t, texture *tex) {
  shader02->use();
  texture_gl_es20 *texture = static_cast<texture_gl_es20 *>(tex);
  texture->bind();
  shader02->set_uniform("s_texture", texture);
  // positions
  gl::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                            &t.v[0].pos);
  GL_CHECK();
  gl::glEnableVertexAttribArray(0);
  GL_CHECK();

  // colors
  gl::glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                            &t.v[0].c);
  GL_CHECK();
  gl::glEnableVertexAttribArray(1);
  GL_CHECK();

  // texture coordinates
  gl::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                            &t.v[0].uv);
  GL_CHECK();
  gl::glEnableVertexAttribArray(2);
  GL_CHECK();

  glDrawArrays(GL_TRIANGLES, 0, 3);
  GL_CHECK();

  gl::glDisableVertexAttribArray(1);
  GL_CHECK();
  gl::glDisableVertexAttribArray(2);
  GL_CHECK();
}
void engine_impl::render(const tri2 &t, texture *tex, const mat3x2 &m) {
  shader02->use();
  texture_gl_es20 *texture = static_cast<texture_gl_es20 *>(tex);
  texture->bind();
  shader02->set_uniform("s_texture", texture);

  shader02->set_uniform("u_matrix", m);

  // positions
  gl::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                            &t.v[0].pos);
  GL_CHECK();
  gl::glEnableVertexAttribArray(0);
  GL_CHECK();

  // colors
  gl::glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                            &t.v[0].c);
  GL_CHECK();
  gl::glEnableVertexAttribArray(1);
  GL_CHECK();

  // texture coordinates
  gl::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                            &t.v[0].uv);
  GL_CHECK();
  gl::glEnableVertexAttribArray(2);
  GL_CHECK();

  glDrawArrays(GL_TRIANGLES, 0, 3);
  GL_CHECK();

  gl::glDisableVertexAttribArray(1);
  GL_CHECK();
  gl::glDisableVertexAttribArray(2);
  GL_CHECK();
}

void engine_impl::render(const tri2 &t, texture *tex, const mat3x2 &m_rotate,
                         const mat3x2 &m_move) {
  shader_matrix->use();
  texture_gl_es20 *texture = static_cast<texture_gl_es20 *>(tex);
  texture->bind();
  shader_matrix->set_uniform("s_texture", texture);

  shader_matrix->set_uniform("u_rotate_matrix", m_rotate);
  shader_matrix->set_uniform("u_move_matrix", m_move);

  // positions
  gl::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                            &t.v[0].pos);
  GL_CHECK();
  gl::glEnableVertexAttribArray(0);
  GL_CHECK();

  // colors
  gl::glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                            &t.v[0].c);
  GL_CHECK();
  gl::glEnableVertexAttribArray(1);
  GL_CHECK();

  // texture coordinates
  gl::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                            &t.v[0].uv);
  GL_CHECK();
  gl::glEnableVertexAttribArray(2);
  GL_CHECK();

  glDrawArrays(GL_TRIANGLES, 0, 3);
  GL_CHECK();

  gl::glDisableVertexAttribArray(1);
  GL_CHECK();
  gl::glDisableVertexAttribArray(2);
  GL_CHECK();
}

void engine_impl::swap_buffers() {
  SDL_GL_SwapWindow(window);

  glClear(GL_COLOR_BUFFER_BIT);
  GL_CHECK();
}
void engine_impl::uninitialize() {
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
} // namespace tme
