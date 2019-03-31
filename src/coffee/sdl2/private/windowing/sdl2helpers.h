#pragma once

#define SDL2_WINDOWING

#include "../types/sdl2datatypes.h"
#include <coffee/core/types/display/properties.h>

namespace Coffee {
namespace SDL2 {

using namespace Display;

extern Uint32 InterpretWindowFlags(const Properties::State& flags);

extern void SetWindowFlags(
    SDL_Window* window, Properties::State const& state);
extern Properties::State GetWindowFlags(SDL_Window* win);

extern void         SetContextProperties(const GL::Properties& props);
extern GL::Properties GetContextProperties();

extern Window* GetWindow(SDL_Window* window);

} // namespace SDL2
} // namespace Coffee
