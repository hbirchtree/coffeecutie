#include "sdl2helpers.h"

#include "sdl2windowsystem.h"

#include "coffee/core/base/cdebug.h"

namespace Coffee{
namespace CSDL2Types{

//"Private" functions
Uint32 _coffee_sdl2_toggle_flag(bool flag, Uint32 value)
{
    if(flag)
        return value;
    else
        return 0;
}

int _coffee_sdl2_get_gl_attribute(SDL_GLattr attr)
{
    int t=0;
    SDL_GL_GetAttribute(attr,&t);
    return t;
}

void _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GLattr attr, uint8* target)
{
    *target = _coffee_sdl2_get_gl_attribute(attr);
}

//Exported functions

Uint32 coffee_sdl2_interpret_winflags(CDWindowProperties::State const& flags)
{
    Uint32 res = 0;

    if(flags&CDWindowProperties::Visible)
        res|=SDL_WINDOW_SHOWN;
    else
        res|=SDL_WINDOW_HIDDEN;

    res |= _coffee_sdl2_toggle_flag(
                flags&CDWindowProperties::State::Undecorated,SDL_WINDOW_BORDERLESS);

    res |= _coffee_sdl2_toggle_flag(
                flags&CDWindowProperties::State::FullScreen,SDL_WINDOW_FULLSCREEN);
    res |= _coffee_sdl2_toggle_flag(
                flags&CDWindowProperties::State::WindowedFullScreen,SDL_WINDOW_FULLSCREEN_DESKTOP);

    res |= _coffee_sdl2_toggle_flag(
                flags&CDWindowProperties::State::Resizable,SDL_WINDOW_RESIZABLE);
    res |= _coffee_sdl2_toggle_flag(
                flags&CDWindowProperties::State::Minimized,SDL_WINDOW_MINIMIZED);
    res |= _coffee_sdl2_toggle_flag(
                flags&CDWindowProperties::State::Maximized,SDL_WINDOW_MAXIMIZED);

    return res;
}

void coffee_sdl2_set_context_properties(const CGLContextProperties &props)
{
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,props.bits.accum.r);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,props.bits.accum.g);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,props.bits.accum.b);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,props.bits.accum.a);

    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,props.bits.alpha);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,props.bits.stencil);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,props.bits.depth);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,props.bits.red);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,props.bits.green);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,props.bits.blue);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,props.version.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,props.version.minor);

    if(props.flags&CGLContextProperties::Flags::GLCoreProfile)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);

    int32 cflags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

    if(props.flags&CGLContextProperties::Flags::GLDebug)
        cflags|=SDL_GL_CONTEXT_DEBUG_FLAG;

    if(props.flags&CGLContextProperties::Flags::GLRobust)
        cflags|=SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,cflags);
}

CGLContextProperties coffee_sdl2_get_context_properties()
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

CDWindowProperties::State coffee_sdl2_get_winflags(SDL_Window *win)
{
    Uint32 flags = SDL_GetWindowFlags(win);
    CDWindowProperties::State res = CDWindowProperties::State(0);

    if(flags&SDL_WINDOW_BORDERLESS)
        res|=CDWindowProperties::Undecorated;

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

void coffee_sdl2_set_winflags(SDL_Window* window,CDWindowProperties::State const& state)
{
    if(!(state&CDWindowProperties::Undecorated))
        SDL_SetWindowBordered(window,SDL_TRUE);
    else
        SDL_SetWindowBordered(window,SDL_FALSE);

    if(state&CDWindowProperties::FullScreen)
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
    else if(state&CDWindowProperties::WindowedFullScreen)
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
    else
        SDL_SetWindowFullscreen(window,0);

    if(state&CDWindowProperties::Minimized)
        SDL_MinimizeWindow(window);
    if(state&CDWindowProperties::Normal)
        SDL_RestoreWindow(window);
    if(state&CDWindowProperties::Maximized)
        SDL_MaximizeWindow(window);

    if(state&CDWindowProperties::Focused)
        SDL_RaiseWindow(window);
}

CDWindow *coffee_sdl2_get_window(SDL_Window *window)
{
    CDWindow* cwin = new CDWindow;

    SDL_GetWindowSize(window,&cwin->screenArea.w,&cwin->screenArea.h);
    SDL_GetWindowPosition(window,&cwin->screenArea.x,&cwin->screenArea.y);

    cwin->title = SDL_GetWindowTitle(window);
    coffee_sdl2_get_window_ptr(window,cwin);

    return cwin;
}

}
}
