#include "engine.hxx"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include "./../nlohmann_json/json.hpp"

using json = nlohmann::json;

enum class log_level { log_warning, log_error };

class Logger {
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

  if (SDL_PollEvent(&sdl_event)) {
    if (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) {
      (sdl_event.type == SDL_KEYDOWN) ? event.action = TME::Action::move_start
                                      : event.action = TME::Action::move_end;

      std::string button(SDL_GetKeyName(sdl_event.key.keysym.sym));
      if (buttons.count(button)) {
        event.move = buttons[button];
        return true;
      }
    }
  }
  return false;
}

int TME::finish() {
  SDL_Quit();
  return EXIT_SUCCESS;
}

Logger::Logger(const std::string& log_file) {
  if (log_file.empty())
    log.open("Engine.log");
  else
    log.open(log_file);
}

Logger::~Logger() {
  log.close();
}

Logger& Logger::get_instance(const std::string& log_file) {
  static Logger instance(log_file);
  return instance;
}

void Logger::add_log(const std::string& log_string, log_level level) {
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
