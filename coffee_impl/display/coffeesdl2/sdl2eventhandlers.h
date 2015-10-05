#ifndef SDL2EVENTHANDLERS
#define SDL2EVENTHANDLERS

#include "sdl2datatypes.h"
#include "coffee_impl/display/csdl2renderer.h"
#include "coffee_impl/input/cinput_eventhandling.h"

namespace Coffee{
namespace CSDL2Types{

extern void coffee_sdl2_eventhandle_all(CSDL2Renderer* ctxt,const SDL_Event* ev);

}
}
#endif // SDL2EVENTHANDLERS


