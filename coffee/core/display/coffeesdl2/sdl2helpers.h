#ifndef SDL2HELPERS_H
#define SDL2HELPERS_H

#define SDL2_WINDOWING

#include "sdl2datatypes.h"
#include "coffee/core/plat/plat_wm.h"
#include "coffee/core/plat/platform_detect.h"
#include "coffee/core/base/cdisplay.h"

namespace Coffee{
namespace CSDL2Types{

extern Uint32 coffee_sdl2_interpret_winflags(const CDWindowProperties::State &flags);

extern void coffee_sdl2_set_winflags(SDL_Window* window, CDWindowProperties::State const& state);
extern CDWindowProperties::State coffee_sdl2_get_winflags(SDL_Window* win);

extern void coffee_sdl2_set_context_properties(const CGLContextProperties& props);
extern CGLContextProperties coffee_sdl2_get_context_properties();

extern CDWindow* coffee_sdl2_get_window(SDL_Window* window);

}
}

#endif // SDL2HELPERS_H
