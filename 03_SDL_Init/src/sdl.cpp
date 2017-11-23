#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

std::ostream& operator<<(std::ostream& out, const SDL_version& v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

int main(int /*argc*/, char* /*argv*/ []) {
  using namespace std;

  ofstream log_file("SDL.log");

  SDL_version compiled{0, 0, 0};
  SDL_version linked{0, 0, 0};
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  log_file << "Compiled SDL version: " << compiled << ".\n";
  log_file << "Linked SDL version: " << linked << "." << endl;

  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    log_file << "Warning, compiled SDL version differs from linked SDL version."
             << endl;
  }

  const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
  if (init_result != 0) {
    const char* err = SDL_GetError();
    log_file << "Error: failed call SDL_Init: " << err << "." << endl;
    return EXIT_FAILURE;
  }

  atexit(SDL_Quit);

  SDL_Window* const window =
      SDL_CreateWindow("First SDL Window", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    const char* err = SDL_GetError();
    log_file << "Error: failed call SDL_CreateWindow: " << err << "." << endl;
    return EXIT_FAILURE;
  }

  const set<SDL_Keycode> buttons = {SDLK_ESCAPE, SDLK_RETURN, SDLK_RIGHT,
                                    SDLK_LEFT,   SDLK_UP,     SDLK_DOWN,
                                    SDLK_f,      SDLK_d};

  bool continue_loop = true;
  string string_event("");

  while (continue_loop) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
      string_event.clear();
      string_event += "Action: ";

      if (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) {
        (sdl_event.type == SDL_KEYDOWN) ? string_event += "key pressed"
                                        : string_event += "key released";

        string_event += ", button: ";

        if (buttons.count(sdl_event.key.keysym.sym)) {
          string_event += SDL_GetKeyName(sdl_event.key.keysym.sym);
          cout << string_event << endl;
          if (sdl_event.key.keysym.sym == SDLK_ESCAPE) {
            continue_loop = false;
          }
          continue;
        } else {
          string_event = string_event + "unhandled button " +
                         SDL_GetKeyName(sdl_event.key.keysym.sym);
        }
      } else {
        string_event += "unhandled action, type: " + to_string(sdl_event.type);
      }
      log_file << string_event << endl;
    }
  }
  return EXIT_SUCCESS;
}
