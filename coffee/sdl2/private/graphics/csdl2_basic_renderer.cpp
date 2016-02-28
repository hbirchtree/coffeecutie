#include <coffee/sdl2/graphics/csdl2_basic_renderer.h>

#include <coffee/core/CProfiling>
#include <coffee/core/coffee_strings.h>
#include "../types/sdl2datatypes.h"

namespace Coffee{
namespace CDisplay{

void SDL2SpriteRenderer::spritesPreInit()
{
    if(SDL_InitSubSystem(SDL_INIT_VIDEO)<0)
    {
        cLog(__FILE__,__LINE__,CFStrings::SDL2_Library_Name,
             CFStrings::SDL2_Library_FailureInit,SDL_GetError());
    }
    Profiler::Profile("Initialize sprite rendering");
}

void SDL2SpriteRenderer::spritesInit()
{
}

void SDL2SpriteRenderer::spritesPostInit()
{
}

void SDL2SpriteRenderer::spritesTerminate()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

}
}