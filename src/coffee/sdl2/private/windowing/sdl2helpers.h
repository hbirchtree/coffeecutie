#pragma once

#define SDL2_WINDOWING

#include "../types/sdl2datatypes.h"

#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/plat/platform_detect.h>

#include <coffee/core/plat/windowmanager.h>

namespace Coffee {
namespace SDL2 {

extern Uint32 InterpretWindowFlags(const CDProperties::State& flags);

extern void SetWindowFlags(
    SDL_Window* window, CDProperties::State const& state);
extern CDProperties::State GetWindowFlags(SDL_Window* win);

extern void         SetContextProperties(const GLProperties& props);
extern GLProperties GetContextProperties();

extern CDWindow* GetWindow(SDL_Window* window);

} // namespace SDL2
} // namespace Coffee
