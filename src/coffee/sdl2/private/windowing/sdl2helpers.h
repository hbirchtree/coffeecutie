#ifndef SDL2HELPERS_H
#define SDL2HELPERS_H

#define SDL2_WINDOWING

#include "../types/sdl2datatypes.h"
#include <coffee/core/plat/plat_windowmanager.h>
#include <coffee/core/plat/platform_detect.h>
#include <coffee/core/base/types/cdisplay.h>

namespace Coffee{
namespace SDL2{

extern Uint32 InterpretWindowFlags(const CDProperties::State &flags);

extern void SetWindowFlags(SDL_Window* window, CDProperties::State const& state);
extern CDProperties::State GetWindowFlags(SDL_Window* win);

extern void SetContextProperties(const GLProperties& props);
extern GLProperties GetContextProperties();

extern CDWindow* GetWindow(SDL_Window* window);

}
}

#endif // SDL2HELPERS_H
