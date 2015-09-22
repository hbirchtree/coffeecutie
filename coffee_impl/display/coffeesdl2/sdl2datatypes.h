#ifndef SDL2DATATYPES
#define SDL2DATATYPES

#include <thread>
#include "coffee/cdisplay.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

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
};

static Uint32 _coffee_sdl2_toggle_flag(bool flag, Uint32 value)
{
    if(flag)
        return value;
    else
        return 0;
}

static Uint32 coffee_sdl2_interpret_winflags(uint32 flags)
{
    Uint32 res;

    if(flags&CDWindowProperties::Visible)
        res|=SDL_WINDOW_SHOWN;
    else
        res|=SDL_WINDOW_HIDDEN;

    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::Decorated,SDL_WINDOW_BORDERLESS);

    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::FullScreen,SDL_WINDOW_FULLSCREEN);
    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::WindowedFullScreen,
                                    SDL_WINDOW_FULLSCREEN_DESKTOP);

    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::Resizable,SDL_WINDOW_RESIZABLE);
    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::Minimized,SDL_WINDOW_MINIMIZED);
    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::Maximized,SDL_WINDOW_MAXIMIZED);

    return res;
}

static void coffee_sdl2_set_context_properties(const CGLContextProperties& props)
{
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,props.bits.alpha);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,props.bits.stencil);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,props.bits.depth);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,props.bits.red);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,props.bits.green);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,props.bits.blue);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,props.version.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,props.version.minor);

    if(props.flags&CGLContextProperties::GLCoreProfile)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);

    int32 cflags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

    if(props.flags&CGLContextProperties::GLDebug)
        cflags|=SDL_GL_CONTEXT_DEBUG_FLAG;

    if(props.flags&CGLContextProperties::GLRobust)
        cflags|=SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,cflags);
}

}
}

#endif // SDL2DATATYPES

