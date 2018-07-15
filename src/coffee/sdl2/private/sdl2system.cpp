#include <coffee/sdl2/sdl2system.h>

#include <coffee/core/coffee_strings.h>
#include <coffee/core/CProfiling>
#include "types/sdl2datatypes.h"

#include <coffee/core/CDebug>

namespace Coffee{
namespace SDL2{

bool SDL2::Init()
{
    /* Initialize SDL2 library, takes some time */
    DProfContext a("SDL2 Init");
    if(SDL_Init(0)<0)
    {
        cLog(__FILE__,__LINE__,CFStrings::SDL2_Library_Name,
             CFStrings::SDL2_Library_FailureInit,SDL_GetError());
        return false;
    }
    Profiler::DeepProfile("SDL2 Initialization");
    return true;
}

bool SDL2::Deinit()
{
    SDL_Quit();
	return true;
}

bool SDL2::Process()
{
	return true;
}

}
}
