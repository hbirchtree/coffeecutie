#ifndef SDL2DATATYPES
#define SDL2DATATYPES

#include <mutex>
#include <thread>
#include "coffee/core/base/cdisplay.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>
#include <graphics_apis/include/gltypes.h>

namespace Coffee{
namespace CSDL2Types{

using namespace CDisplay;

struct CSDL2Context{
    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;

    CGL::CGL_Context* context;

    SDL_Event eventhandle;
    uint8 contextFlags = 0;

    std::thread::id thread;

    std::mutex controller_access;
    std::map<uint8,SDL_GameController*> controllers;
    std::map<uint8,SDL_Haptic*> haptics;
};

}
}

#endif // SDL2DATATYPES

