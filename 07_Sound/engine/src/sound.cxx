#include "engine.hxx"
#include <SDL2/SDL.h>
#include <stdexcept>

namespace tme {

sound::sound(const std::string &file)
    : device_id(0), buffer(nullptr), buffer_size(0) {

  if (!load(file))
    throw std::runtime_error("can't load sound");
}

bool sound::load(const std::string &file) {
  SDL_AudioSpec wavSpec;

  if (SDL_LoadWAV(file.c_str(), &wavSpec, &buffer, &buffer_size) == NULL)
    return false;

  device_id = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
  if (!device_id)
    return false;

  return true;
}
void sound::play() const {
  SDL_QueueAudio(device_id, buffer, buffer_size);
  SDL_PauseAudioDevice(device_id, 0);
}
void sound::play_always() const {
  constexpr uint32_t max_play = 20;
  for (uint32_t i = 0; i < max_play; ++i)
    SDL_QueueAudio(device_id, buffer, buffer_size);
  SDL_PauseAudioDevice(device_id, 0);
}
void sound::stop() const { SDL_PauseAudioDevice(device_id, 1); }
sound::~sound() {
  SDL_CloseAudioDevice(device_id);
  SDL_FreeWAV(buffer);
}

} // namespace tme
