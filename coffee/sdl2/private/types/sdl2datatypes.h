#ifndef SDL2DATATYPES
#define SDL2DATATYPES

#include <coffee/core/base/types/cdisplay.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>
#include <coffee/graphics_apis/gltypes.h>

namespace Coffee{
namespace CSDL2Types{

using namespace CDisplay;

struct CSDL2Context{
    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;

    CGL::CGL_Context* context;

    Vector<CGL::CGL_Context*> m_subcontexts;

    SDL_Event eventhandle;
    uint8 contextFlags = 0;

    ThreadId thread;

    Mutex controller_access;
    Map<uint8,SDL_GameController*> controllers;
    Map<uint8,SDL_Haptic*> haptics;
};

}
}

#endif // SDL2DATATYPES

