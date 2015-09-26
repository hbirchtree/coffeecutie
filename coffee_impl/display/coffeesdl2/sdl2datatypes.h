#ifndef SDL2DATATYPES
#define SDL2DATATYPES

#include <thread>
#include "coffee/cdisplay.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>

namespace Coffee{
namespace CSDL2Types{

using namespace CDisplay;

struct CSDL2Context{
    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;

    SDL_GLContext context;

    SDL_Event eventhandle;
    uint8 contextFlags = 0;

    std::thread::id thread;

    std::map<byte,SDL_GameController*> controllers;
};

}
}

#endif // SDL2DATATYPES

