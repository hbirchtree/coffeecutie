#include "sdl2helpers.h"

#include "sdl2windowsystem.h"

#include <coffee/core/CDebug>
#include <coffee/core/platform_data.h>

namespace Coffee {
namespace SDL2 {

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
    int t = 0;
    SDL_GL_GetAttribute(attr, &t);
    return t;
}

void _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GLattr attr, u8* target)
{
    *target = _coffee_sdl2_get_gl_attribute(attr);
}

// Exported functions

Uint32 InterpretWindowFlags(Properties::State const& flags)
{
    Uint32 res = 0;

    if(flags & Properties::Visible)
        res |= SDL_WINDOW_SHOWN;
    else
        res |= SDL_WINDOW_HIDDEN;

    res |= _coffee_sdl2_toggle_flag(
        flags & Properties::State::Undecorated, SDL_WINDOW_BORDERLESS);

    res |= _coffee_sdl2_toggle_flag(
        flags & Properties::State::FullScreen, SDL_WINDOW_FULLSCREEN);
    res |= _coffee_sdl2_toggle_flag(
        flags & Properties::State::WindowedFullScreen,
        SDL_WINDOW_FULLSCREEN_DESKTOP);

    res |= _coffee_sdl2_toggle_flag(
        flags & Properties::State::Resizable, SDL_WINDOW_RESIZABLE);
    res |= _coffee_sdl2_toggle_flag(
        flags & Properties::State::Minimized, SDL_WINDOW_MINIMIZED);
    res |= _coffee_sdl2_toggle_flag(
        flags & Properties::State::Maximized, SDL_WINDOW_MAXIMIZED);
    res |= _coffee_sdl2_toggle_flag(
        flags & Properties::State::Visible, SDL_WINDOW_SHOWN);

#if defined(COFFEE_APPLE)
    res |= SDL_WINDOW_ALLOW_HIGHDPI;
#endif

    return res;
}

void SetContextProperties(const GL::Properties& props)
{
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);

    if(props.flags & GL::Properties::GLSRGB)
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);

    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, props.bits.alpha);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, props.bits.stencil);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, props.bits.depth);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, props.bits.red);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, props.bits.green);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, props.bits.blue);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, props.version.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, props.version.minor);

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDL_FALSE);

    if(props.flags & GL::Properties::Flags::GLCoreProfile)
    {
        if(props.flags & GL::Properties::Flags::GLES)
            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        else
            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    }

    i32 cflags = 0;

    if(props.flags & GL::Properties::Flags::GLDebug)
        cflags |= SDL_GL_CONTEXT_DEBUG_FLAG;

    if(props.flags & GL::Properties::Flags::GLRobust)
        cflags |= SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;

#if defined(COFFEE_APPLE)
    cflags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#endif

    if(!(props.flags & GL::Properties::Flags::GLES))
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, cflags);
}

GL::Properties GetContextProperties()
{
    GL::Properties props;

    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_RED_SIZE, &props.bits.red);
    _coffee_sdl2_get_gl_attribute_to_uint8(
        SDL_GL_GREEN_SIZE, &props.bits.green);
    _coffee_sdl2_get_gl_attribute_to_uint8(SDL_GL_BLUE_SIZE, &props.bits.blue);
    _coffee_sdl2_get_gl_attribute_to_uint8(
        SDL_GL_ALPHA_SIZE, &props.bits.alpha);

    _coffee_sdl2_get_gl_attribute_to_uint8(
        SDL_GL_STENCIL_SIZE, &props.bits.stencil);
    _coffee_sdl2_get_gl_attribute_to_uint8(
        SDL_GL_DEPTH_SIZE, &props.bits.depth);

    props.version.major =
        _coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_MAJOR_VERSION);
    props.version.minor =
        _coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_MINOR_VERSION);

    if(_coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_PROFILE_MASK) ==
       SDL_GL_CONTEXT_PROFILE_CORE)
        props.flags |= GL::Properties::GLCoreProfile;

    i32 cflags = _coffee_sdl2_get_gl_attribute(SDL_GL_CONTEXT_FLAGS);

    if(cflags & SDL_GL_CONTEXT_DEBUG_FLAG)
        props.flags |= GL::Properties::GLDebug;
    if(cflags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG)
        props.flags |= GL::Properties::GLRobust;

    return props;
}

Properties::State GetWindowFlags(SDL_Window* win)
{
    Uint32              flags = SDL_GetWindowFlags(win);
    Properties::State res   = Properties::State(0);

    if(flags & SDL_WINDOW_BORDERLESS)
        res = res | Properties::Undecorated;

    if(flags & SDL_WINDOW_FULLSCREEN)
        res = res | Properties::FullScreen;

    else if(flags & SDL_WINDOW_BORDERLESS)
        res = res | Properties::WindowedFullScreen;
    else
        res = res | Properties::Windowed;

    if(flags & SDL_WINDOW_RESIZABLE)
        res = res | Properties::Resizable;

    if(flags & SDL_WINDOW_MINIMIZED)
        res = res | Properties::Minimized;
    if(flags & SDL_WINDOW_MAXIMIZED)
        res = res | Properties::Maximized;

    if(flags & SDL_WINDOW_SHOWN)
        res = res | Properties::Visible;

    if(flags & SDL_WINDOW_INPUT_FOCUS)
        res = res | Properties::Focused;

    if(flags & SDL_WINDOW_FOREIGN)
        res = res | Properties::Foreign;

    return res;
}

void SetWindowFlags(SDL_Window* window, Properties::State const& state)
{
    if(!(state & Properties::Undecorated))
        SDL_SetWindowBordered(window, SDL_TRUE);
    else
        SDL_SetWindowBordered(window, SDL_FALSE);

    if(state & Properties::FullScreen)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    else if(state & Properties::WindowedFullScreen)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    else
    {
        SDL_SetWindowBordered(window, SDL_TRUE);
        SDL_SetWindowFullscreen(window, 0);
    }

    if(state & Properties::Minimized)
        SDL_MinimizeWindow(window);
    if(state & Properties::Normal)
        SDL_RestoreWindow(window);
    if(state & Properties::Maximized)
        SDL_MaximizeWindow(window);

    if(state & Properties::Focused)
        SDL_RaiseWindow(window);
}

Window* GetWindow(SDL_Window* window)
{
    Window* cwin = new Window;
    GetWindowPtr(window, cwin);

    return cwin;
}

} // namespace SDL2
} // namespace Coffee
