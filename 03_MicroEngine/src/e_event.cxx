#include "engine.hxx"

using namespace TME;

e_event& e_event::operator=(const e_event& e) {
  action = e.action;
  move = e.move;
}
