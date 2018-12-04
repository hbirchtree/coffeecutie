#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/stl/thread_types.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_video.h>
#include <coffee/graphics/common/gltypes.h>

#define SDLGL_API "SDL2GL::"
#define SDLEV_API "SDL2Event::"
#define SDLWM_API "SDL2Window::"

namespace Coffee {
namespace SDL2 {

struct SDL2Context
{
    SDL_Window*  window  = nullptr;
    SDL_Surface* surface = nullptr;

#if defined(COFFEE_USE_SDL_GL)
    CGL::CGL_Context* context = nullptr;
#endif

    //    Vector<CGL::CGL_Context*> m_subcontexts;

    u8 contextFlags = 0;

    ThreadId thread;

    Map<u8, SDL_GameController*> controllers;
    Map<u8, SDL_Joystick*>       joysticks;
    Map<u8, SDL_Haptic*>         haptics;
};

} // namespace SDL2
} // namespace Coffee
