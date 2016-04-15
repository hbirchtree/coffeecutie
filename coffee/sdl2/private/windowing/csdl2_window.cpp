#include <coffee/sdl2/windowing/csdl2_window.h>

#include <coffee/core/CDebug>
#include <coffee/core/CFiles>

#include "sdl2helpers.h"

#include <SDL2/SDL_opengles2.h>

namespace Coffee{
namespace Display{

bool SDL2Window::windowPreInit(const CDProperties& p, CString *)
{
    m_window_flags = 0;

    if(SDL_InitSubSystem(SDL_INIT_VIDEO)<0)
    {
        cLog(__FILE__,__LINE__,CFStrings::SDL2_Library_Name,
             CFStrings::SDL2_Library_FailureInit,SDL_GetError());
        return false;
    }

    /* Create SDL2 context object */
    setSDL2Context(new Context);
    new (&getSDL2Context()->thread) ThreadId;

    SDL_version ver;
    SDL_GetVersion(&ver);
    m_contextString = cStringFormat("SDL {0}.{1}.{2}",ver.major,ver.minor,ver.patch);

    Profiler::Profile("Create SDL2 context");
    return true;
}

bool SDL2Window::windowInit(const CDProperties& p, CString* err)
{
    /* Translate window flags and apply them */
    m_window_flags |= CSDL2Types::InterpretWindowFlags(p.flags);

    /* Create the platform window */
    const CSize& winsize = p.size;
    const cstring& wintitle = p.title;

    getSDL2Context()->window =
            SDL_CreateWindow(wintitle,
                             SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                             winsize.w,winsize.h,
                             m_window_flags);


    /* Validate the window pointer, may have failed */
    if(!getSDL2Context()->window){
        if(err)
            *err = cStringFormat(CFStrings::SDL2_Library_FailureInit,SDL_GetError());
        return false;
    }
    Profiler::Profile("Create window");
    return true;
}

bool SDL2Window::windowPostInit(const CDProperties& p, CString *)
{
    /* If an icon pointer is present, use it */
    if(p.icon)
    {
        setWindowIcon(*p.icon);
    }

    /* Show window if requested */
    if(p.flags&CDProperties::Visible)
        showWindow();

    /* Set windowed fullscreen if requested */
    if(p.flags&CDProperties::Windowed)
        SDL_SetWindowFullscreen(getSDL2Context()->window,0);

    cMsg("SDL2","Running {0}",m_contextString);

    Profiler::Profile("Set window properties");
    return true;
}

void SDL2Window::windowTerminate()
{
    if(!getSDL2Context())
        return;

    /* Delete window */
    SDL_DestroyWindow(getSDL2Context()->window);
    delete getSDL2Context();
    setSDL2Context(nullptr);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

CSize SDL2Window::windowSize() const
{
    CSize size = {};
    SDL_GetWindowSize(getSDL2Context()->window,&size.w,&size.h);
    return size;
}

void SDL2Window::setWindowSize(const CSize &size)
{
    SDL_SetWindowSize(getSDL2Context()->window,size.w,size.h);
    SDL_SetWindowMinimumSize(getSDL2Context()->window,size.w,size.h);
    SDL_SetWindowMaximumSize(getSDL2Context()->window,size.w,size.h);
}

bool SDL2Window::setWindowIcon(CBitmap &icon)
{

    SDL_Surface* sdl_icon = SDL_CreateRGBSurfaceFrom(icon.data(),
                                                     icon.size.w,icon.size.h,
                                                     32,sizeof(CRGBA),
                                                     0,0,0,0);
    if(!sdl_icon)
        return false;
    SDL_SetWindowIcon(getSDL2Context()->window,sdl_icon);
    SDL_FreeSurface(sdl_icon);
    return true;
}

CDWindow* SDL2Window::window()
{
    return CSDL2Types::GetWindow(getSDL2Context()->window);
}

uint32 SDL2Window::windowState() const
{
    return CSDL2Types::GetWindowFlags(getSDL2Context()->window);
}

void SDL2Window::setWindowState(const CDProperties::State &state)
{
    CSDL2Types::SetWindowFlags(getSDL2Context()->window,state);
}

bool SDL2Window::screensaverMode()
{
    return SDL_IsScreenSaverEnabled();
}

void SDL2Window::setScreensaverMode(bool state)
{
    if(state)
        SDL_EnableScreenSaver();
    else
        SDL_DisableScreenSaver();
}

CPoint SDL2Window::windowPosition() const
{
    CPoint pos;
    SDL_GetWindowSize(getSDL2Context()->window,&pos.x,&pos.y);
    return pos;
}

void SDL2Window::setWindowPosition(const CPoint& pos)
{
    SDL_SetWindowPosition(getSDL2Context()->window,pos.x,pos.y);
}

bool SDL2Window::showWindow()
{
    SDL_ShowWindow(getSDL2Context()->window);
    return true;
}

bool SDL2Window::hideWindow()
{
    SDL_HideWindow(getSDL2Context()->window);
    return true;
}

void SDL2Window::popErrorMessage(Severity s, cstring title, cstring msg)
{
    Uint32 flags = 0;

    switch(s)
    {
    case Severity::Critical:
    case Severity::Fatal:
    case Severity::High:
        flags |= SDL_MESSAGEBOX_ERROR;
        break;
    case Severity::Low:
    case Severity::Medium:
        flags |= SDL_MESSAGEBOX_WARNING;
        break;
    case Severity::Information:
        flags |= SDL_MESSAGEBOX_INFORMATION;
        break;
    case Severity::Debug:
        flags |= SDL_MESSAGEBOX_INFORMATION;
        break;
    }

    SDL_ShowSimpleMessageBox(flags,title,msg,NULL);
}

bool SDL2Window::closeWindow()
{
    getSDL2Context()->contextFlags |= 0x1;
    return true;
}

CString SDL2Window::windowTitle() const
{
    return SDL_GetWindowTitle(getSDL2Context()->window);
}

void SDL2Window::setWindowTitle(const CString &title)
{
    SDL_SetWindowTitle(getSDL2Context()->window,title.c_str());
}

CDMonitor SDL2Window::monitor()
{
    CDMonitor mon;

    SDL_DisplayMode dm;
    if(SDL_GetCurrentDisplayMode(initialProperties().monitor,&dm)<0)
        cDebug("Failed to get monitor information: {0}",SDL_GetError());
    else{

        mon.refresh = dm.refresh_rate;
        mon.screenArea.w = dm.w;
        mon.screenArea.h = dm.h;
        mon.name = SDL_GetDisplayName(initialProperties().monitor);
        mon.index = initialProperties().monitor;

        //TODO: Add color space
        //TODO: Add physical dimensions
        //TODO: Add screenArea offsets
    }

    return mon;
}

}
}

