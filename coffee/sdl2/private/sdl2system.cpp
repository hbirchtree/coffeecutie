#include <coffee/sdl2/sdl2system.h>

#include <coffee/core/coffee_strings.h>
#include <coffee/core/CDebug>
#include "types/sdl2datatypes.h"

namespace Coffee{
namespace SDL2{

bool SDL2::Init()
{
    /* Initialize SDL2 library, takes some time */
    Profiler::PushContext("SDL2 Init");
    if(SDL_Init(0)<0)
    {
        cLog(__FILE__,__LINE__,CFStrings::SDL2_Library_Name,
             CFStrings::SDL2_Library_FailureInit,SDL_GetError());
        Profiler::PopContext();
        return false;
    }
    Profiler::Profile("SDL2 Initialization");
    Profiler::PopContext();
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