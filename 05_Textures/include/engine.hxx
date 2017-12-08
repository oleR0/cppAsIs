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

namespace tme { // Tanks MicroEngine

enum class Action
{
  move_start,
  move_end
};

struct e_event
{
  Action action;
  std::string move;

  e_event& operator=(const e_event&);
};

struct vertex
{
  vertex()
    : x(0.f)
    , y(0.f)
	, tx(0.f)
	, ty(0.f)
  {}
  float x;
  float y;

  float tx;
  float ty;
};

struct DECLSPEC triangle
{
  triangle()
    : v{ vertex() }
  {}
  vertex v[3];
};

std::istream& DECLSPEC
operator>>(std::istream& is, vertex&);
std::istream& DECLSPEC
operator>>(std::istream& is, triangle&);

static std::map<std::string, std::string> buttons;

const std::string DECLSPEC
init(const std::string&);
bool DECLSPEC
read_event(e_event&);
int DECLSPEC
finish();

void DECLSPEC
render_triangle(const triangle&);
void DECLSPEC
swap_buffers();
}; // namespace TME
