#ifndef SDL2EVENTHANDLERS
#define SDL2EVENTHANDLERS

#include "sdl2datatypes.h"
#include "coffee/core/display/csdl2renderer.h"

namespace Coffee{
namespace CSDL2Types{

extern void coffee_sdl2_eventhandle_all(CSDL2Renderer* ctxt,const SDL_Event* ev);

}
}
#endif // SDL2EVENTHANDLERS


