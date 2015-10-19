#ifndef SDL2HELPERS_H
#define SDL2HELPERS_H

#define SDL2_WINDOWING

#include "sdl2datatypes.h"
#include "plat/plat_wm.h"
#include "plat/platform_detect.h"
#include "coffee/cdisplay.h"

namespace Coffee{
namespace CSDL2Types{

extern Uint32 coffee_sdl2_interpret_winflags(uint32 flags);
extern void coffee_sdl2_set_winflags(SDL_Window* win,uint32 flags);
extern uint32 coffee_sdl2_get_winflags(SDL_Window* win);
extern void coffee_sdl2_set_context_properties(const CGLContextProperties& props);
extern CGLContextProperties coffee_sdl2_get_context_properties();
extern void coffee_sdl2_get_window_ptr(SDL_Window* window, CDWindow* win);

}
}

#endif // SDL2HELPERS_H

