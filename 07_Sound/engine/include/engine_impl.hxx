#pragma once
#include "engine.hxx"
#include "shader.hxx"
#include <SDL2/SDL.h>

namespace tme {

struct bind {
  bind(std::string_view s, SDL_Keycode k, event pressed, event released)
      : name(s), key(k), event_pressed(pressed), event_released(released) {}

  std::string_view name;
  SDL_Keycode key;

  event event_pressed;
  event event_released;
};

const std::array<bind, 8> keys{
    {bind{"up", SDLK_w, event::up_pressed, event::up_released},
     bind{"left", SDLK_a, event::left_pressed, event::left_released},
     bind{"down", SDLK_s, event::down_pressed, event::down_released},
     bind{"right", SDLK_d, event::right_pressed, event::right_released},
     bind{"button1", SDLK_LCTRL, event::button1_pressed,
          event::button1_released},
     bind{"button2", SDLK_SPACE, event::button2_pressed,
          event::button2_released},
     bind{"select", SDLK_ESCAPE, event::select_pressed, event::select_released},
     bind{"start", SDLK_RETURN, event::start_pressed, event::start_released}}};

class engine_impl final : public engine {
public:
  /// create main window
  /// on success return empty string
  std::string initialize(std::string_view /*config*/) final;
  /// return seconds from initialization
  float get_time_from_init() final;
  bool count_to_1(float *const, const int &) final;
  bool read_input(event &e) final;
  texture *create_texture(std::string_view path) final;
  void destroy_texture(texture *t) final;

  void render(const tri0 &t, const color &c) final;
  void render(const tri1 &t) final;
  void render(const tri2 &t, texture *tex) final;
  void render(const tri2 &t, texture *tex, const mat3x2 &m) final;
  void render(const tri2 &t, texture *tex, const mat3x2 &m_rotate,
              const mat3x2 &m_move) final;

  void swap_buffers() final;
  void uninitialize() final;

private:
  SDL_Window *window = nullptr;
  SDL_GLContext gl_context = nullptr;

  shader_gl_es20 *shader00 = nullptr;
  shader_gl_es20 *shader01 = nullptr;
  shader_gl_es20 *shader02 = nullptr;
  shader_gl_es20 *shader_matrix = nullptr;

  static uint32_t counter_function(uint32_t interval, void *param);
};
} // namespace tme
