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

static void coffee_sdl2_get_window_ptr(SDL_Window* window, CDWindow* win)
{
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version)

    if(SDL_GetWindowWMInfo(window,&info)){
        switch(info.subsystem){
#ifdef __linux__
        case SDL_SYSWM_X11:
            win->handle = info.info.x11.window;
            break;
        case SDL_SYSWM_MIR:
            win->handle_p = info.info.mir.surface;
            break;
        case SDL_SYSWM_WAYLAND:
            win->handle_p = info.info.wl.surface;
            break;
#endif
#ifdef _WIN32
        case SDL_SYSWM_WINDOWS:
            break;
#endif
#if defined(__APPLE__) && defined(__MACH__)
        case SDL_SYSWM_COCOA:
            break;
        case SDL_SYSWM_UIKIT:
            break;
#endif
        default:
            break;
        }
    }else{
        cDebug("Failed to acquire information on window: %s",SDL_GetError());
    }
}

}
}

#endif // SDL2DATATYPES

