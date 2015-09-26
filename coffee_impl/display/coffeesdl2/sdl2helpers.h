#ifndef SDL2HELPERS_H
#define SDL2HELPERS_H

#define SDL2_WINDOWING

#include "sdl2datatypes.h"

namespace Coffee{
namespace CSDL2Types{

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
    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::WindowedFullScreen,SDL_WINDOW_FULLSCREEN_DESKTOP);

    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::Resizable,SDL_WINDOW_RESIZABLE);
    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::Minimized,SDL_WINDOW_MINIMIZED);
    res |= _coffee_sdl2_toggle_flag(flags&CDWindowProperties::Maximized,SDL_WINDOW_MAXIMIZED);

    return res;
}

static void coffee_sdl2_set_winflags(SDL_Window* win,uint32 flags)
{
    C_UNUSED(win);
    C_UNUSED(flags);
}

static uint32 coffee_sdl2_get_winflags(SDL_Window* win)
{
    Uint32 flags = SDL_GetWindowFlags(win);
    uint32 res = 0;

    if(flags&SDL_WINDOW_BORDERLESS)
        res^=CDWindowProperties::Decorated;
    else
        res|=CDWindowProperties::Decorated;

    if(flags&SDL_WINDOW_FULLSCREEN)
        res|=CDWindowProperties::FullScreen;
    else if(flags&SDL_WINDOW_FULLSCREEN_DESKTOP)
        res|=CDWindowProperties::WindowedFullScreen;
    else
        res|=CDWindowProperties::Windowed;

    if(flags&SDL_WINDOW_RESIZABLE)
        res|=CDWindowProperties::Resizable;

    if(flags&SDL_WINDOW_MINIMIZED)
        res|=CDWindowProperties::Minimized;
    if(flags&SDL_WINDOW_MAXIMIZED)
        res|=CDWindowProperties::Maximized;

    if(flags&SDL_WINDOW_SHOWN)
        res|=CDWindowProperties::Visible;

    if(flags&SDL_WINDOW_INPUT_FOCUS)
        res|=CDWindowProperties::Focused;

    if(flags&SDL_WINDOW_FOREIGN)
        res|=CDWindowProperties::Foreign;

    return res;
}

static int _coffee_sdl2_get_gl_attribute(SDL_GLattr attr)
{
    int t=0;
    SDL_GL_GetAttribute(attr,&t);
    return t;
}

static void _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GLattr attr, uint8* target)
{
    *target = _coffee_sdl2_get_gl_attribute(attr);
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

static CGLContextProperties coffee_sdl2_get_context_properties()
{
    CGLContextProperties props;

    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_RED_SIZE,&props.bits.red);
    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_GREEN_SIZE,&props.bits.green);
    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_BLUE_SIZE,&props.bits.blue);
    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_ALPHA_SIZE,&props.bits.alpha);

    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_STENCIL_SIZE,&props.bits.stencil);
    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_DEPTH_SIZE,&props.bits.depth);

    props.version.major =
            _coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_MAJOR_VERSION);
    props.version.minor =
            _coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_MINOR_VERSION);

    if(_coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_PROFILE_MASK)==SDL_GL_CONTEXT_PROFILE_CORE)
        props.flags|=CGLContextProperties::GLCoreProfile;

    int32 cflags = _coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_FLAGS);

    if(cflags&SDL_GL_CONTEXT_DEBUG_FLAG)
        props.flags|=CGLContextProperties::GLDebug;
    if(cflags&SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG)
        props.flags|=CGLContextProperties::GLRobust;

    return props;
}

static void coffee_sdl2_get_window_ptr(SDL_Window* window, CDWindow* win)
{
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version)

    if(SDL_GetWindowWMInfo(window,&info)){
        switch(info.subsystem){
#ifdef __linux__
        case SDL_SYSWM_X11:
//            win->handle = info.info.x11.window;
            break;
        case SDL_SYSWM_MIR:
//            win->handle_p = info.info.mir.surface;
            break;
        case SDL_SYSWM_WAYLAND:
//            win->handle_p = info.info.wl.surface;
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

#endif // SDL2HELPERS_H

