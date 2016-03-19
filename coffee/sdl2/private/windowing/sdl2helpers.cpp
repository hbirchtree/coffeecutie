#include "sdl2helpers.h"

#include "sdl2windowsystem.h"

#include <coffee/core/CDebug>

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

Uint32 InterpretWindowFlags(CDProperties::State const& flags)
{
    Uint32 res = 0;

    if(flags&CDProperties::Visible)
        res|=SDL_WINDOW_SHOWN;
    else
        res|=SDL_WINDOW_HIDDEN;

    res |= _coffee_sdl2_toggle_flag(
                flags&CDProperties::State::Undecorated,SDL_WINDOW_BORDERLESS);

    res |= _coffee_sdl2_toggle_flag(
                flags&CDProperties::State::FullScreen,SDL_WINDOW_FULLSCREEN);
    res |= _coffee_sdl2_toggle_flag(
                flags&CDProperties::State::WindowedFullScreen,SDL_WINDOW_FULLSCREEN_DESKTOP);

    res |= _coffee_sdl2_toggle_flag(
                flags&CDProperties::State::Resizable,SDL_WINDOW_RESIZABLE);
    res |= _coffee_sdl2_toggle_flag(
                flags&CDProperties::State::Minimized,SDL_WINDOW_MINIMIZED);
    res |= _coffee_sdl2_toggle_flag(
                flags&CDProperties::State::Maximized,SDL_WINDOW_MAXIMIZED);
    res |= _coffee_sdl2_toggle_flag(
                flags&CDProperties::State::Visible,SDL_WINDOW_SHOWN);

    return res;
}

void SetContextProperties(const GLProperties &props)
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

    if(props.flags&GLProperties::Flags::GLCoreProfile)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);

    int32 cflags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

    if(props.flags&GLProperties::Flags::GLDebug)
        cflags|=SDL_GL_CONTEXT_DEBUG_FLAG;

    if(props.flags&GLProperties::Flags::GLRobust)
        cflags|=SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,cflags);
}

GLProperties GetContextProperties()
{
    GLProperties props;

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
        props.flags|=GLProperties::GLCoreProfile;

    int32 cflags = _coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_FLAGS);

    if(cflags&SDL_GL_CONTEXT_DEBUG_FLAG)
        props.flags|=GLProperties::GLDebug;
    if(cflags&SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG)
        props.flags|=GLProperties::GLRobust;

    return props;
}

CDProperties::State GetWindowFlags(SDL_Window *win)
{
    Uint32 flags = SDL_GetWindowFlags(win);
    CDProperties::State res = CDProperties::State(0);

    if(flags&SDL_WINDOW_BORDERLESS)
        res=res|CDProperties::Undecorated;

    if(flags&SDL_WINDOW_FULLSCREEN)
        res=res|CDProperties::FullScreen;

    else if(flags&SDL_WINDOW_BORDERLESS)
        res=res|CDProperties::WindowedFullScreen;
    else
        res=res|CDProperties::Windowed;

    if(flags&SDL_WINDOW_RESIZABLE)
        res=res|CDProperties::Resizable;

    if(flags&SDL_WINDOW_MINIMIZED)
        res=res|CDProperties::Minimized;
    if(flags&SDL_WINDOW_MAXIMIZED)
        res=res|CDProperties::Maximized;

    if(flags&SDL_WINDOW_SHOWN)
        res=res|CDProperties::Visible;

    if(flags&SDL_WINDOW_INPUT_FOCUS)
        res=res|CDProperties::Focused;

    if(flags&SDL_WINDOW_FOREIGN)
        res=res|CDProperties::Foreign;

    return res;
}

void SetWindowFlags(SDL_Window* window,CDProperties::State const& state)
{
    if(!(state&CDProperties::Undecorated))
        SDL_SetWindowBordered(window,SDL_TRUE);
    else
        SDL_SetWindowBordered(window,SDL_FALSE);

    if(state&CDProperties::FullScreen)
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
    else if(state&CDProperties::WindowedFullScreen)
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
    else
    {
        SDL_SetWindowBordered(window,SDL_TRUE);
        SDL_SetWindowFullscreen(window,0);
    }

    if(state&CDProperties::Minimized)
        SDL_MinimizeWindow(window);
    if(state&CDProperties::Normal)
        SDL_RestoreWindow(window);
    if(state&CDProperties::Maximized)
        SDL_MaximizeWindow(window);

    if(state&CDProperties::Focused)
        SDL_RaiseWindow(window);
}

CDWindow *GetWindow(SDL_Window *window)
{
    CDWindow* cwin = new CDWindow;

    SDL_GetWindowSize(window,&cwin->screenArea.w,&cwin->screenArea.h);
    SDL_GetWindowPosition(window,&cwin->screenArea.x,&cwin->screenArea.y);

    cwin->title = SDL_GetWindowTitle(window);
    GetWindowPtr(window,cwin);

    return cwin;
}

}
}
