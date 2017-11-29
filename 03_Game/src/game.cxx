#include <iostream>
#include <string>
#include "engine.hxx"

int main(int a, char** b) {
  std::string init_result = TME::init("config.json");
  if (!init_result.empty())
    return EXIT_FAILURE;

  bool continue_loop = true;

  while (continue_loop) {
    TME::e_event event;
    while (TME::read_event(event)) {
      std::cout << ((event.action == TME::Action::move_start) ? "start "
                                                              : "end ")
                << event.move << std::endl;
      if (!event.move.compare("Select")) {
        continue_loop = false;
      }
    }
  }
  return TME::finish();
}
