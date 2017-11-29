#pragma once

#include <fstream>
#include <map>
#include <string>

#ifdef _WIN32
#ifdef BUILD_ENGINE
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#else
#define DECLSPEC
#endif

namespace TME {  // Tanks MicroEngine

enum class Action { move_start, move_end };

struct e_event {
  Action action;
  std::string move;

  e_event& operator=(const e_event&);
};

static std::map<std::string, std::string> buttons;
const std::string DECLSPEC init(const std::string&);
bool DECLSPEC read_event(e_event&);
int DECLSPEC finish();
};  // namespace TME
