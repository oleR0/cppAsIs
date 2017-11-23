#include "engine.hxx"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include "./../nlohmann_json/json.hpp"

using json = nlohmann::json;

const std::string TME::init(const std::string& config_file) {
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

  SDL_version compiled{0, 0, 0};
  SDL_version linked{0, 0, 0};
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

  SDL_Window* const window =
      SDL_CreateWindow("First SDL Window", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    std::string err("failed call SDL_CreateWindow: ");
    err += SDL_GetError();
    logger.add_log(err, log_level::log_error);
    return err;
  }

  return "";
}

bool TME::read_event(TME::e_event& event) {
  SDL_Event sdl_event;
  static std::queue<TME::e_event> good_events;

  //так как нужно знать, есть ли дальше нужный нам event (а не любой),
  //приходится извращаться
  if (good_events.size()) {
    event = good_events.front();
    good_events.pop();
    return (good_events.size()) ? true : false;
  }

  bool good_event = false;
  while (!good_event) {
    while (SDL_PollEvent(&sdl_event)) {
      e_event e;
      if (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) {
        (sdl_event.type == SDL_KEYDOWN) ? e.action = TME::Action::move_start
                                        : e.action = TME::Action::move_end;

        std::string button(SDL_GetKeyName(sdl_event.key.keysym.sym));
        if (buttons.count(button)) {
          e.move = buttons[button];
          good_events.push(e);
          good_event = true;
        }
      }
    }
  }
  return read_event(event);
}

int TME::finish() {
  SDL_Quit();
  return EXIT_SUCCESS;
}
