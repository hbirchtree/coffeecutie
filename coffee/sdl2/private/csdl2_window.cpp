#include <coffee/sdl2/csdl2_window.h>

#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include "windowing/sdl2helpers.h"

namespace Coffee{
namespace CDisplay{

void SDL2Window::windowPreInit(const CDProperties& p)
{
    m_window_flags = 0;

    /* Create SDL2 context object */
    setSDL2Context(new Context);
    new (&getSDL2Context()->thread) ThreadId;

    SDL_version ver;
    SDL_GetVersion(&ver);
    m_contextString = cStringFormat("SDL {0}.{1}.{2}",ver.major,ver.minor,ver.patch);

    Profiler::Profile("Pre-init");
}

void SDL2Window::windowInit(const CDProperties& p)
{
    /* Initialize SDL2 library, takes some time */
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC)<0)
    {
        cFatal("Failed to initialize SDL2 context: {0}",SDL_GetError());
    }
    cMsg("SDL2","Initialized");

    Profiler::Profile("Initialization");

    /* Translate window flags and apply them */
    m_window_flags |= CSDL2Types::coffee_sdl2_interpret_winflags(p.flags);

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
        cFatal("Failed to create SDL2 window: {0}",SDL_GetError());
    }

    Profiler::Profile("Create window");
}

void SDL2Window::windowPostInit(const CDProperties& p)
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

}

void SDL2Window::windowTerminate()
{
    cMsg("SDL2","Cleaning up context");


    /* Delete window */
    SDL_DestroyWindow(getSDL2Context()->window);
    /* De-initialize SDL */
    SDL_Quit();
    delete getSDL2Context();
    setSDL2Context(nullptr);
    cMsg("SDL2","Terminated");
}

CSize SDL2Window::windowSize() const
{
    CSize size;
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
    return CSDL2Types::coffee_sdl2_get_window(getSDL2Context()->window);
}

uint32 SDL2Window::windowState() const
{
    return CSDL2Types::coffee_sdl2_get_winflags(getSDL2Context()->window);
}

void SDL2Window::setWindowState(const CDProperties::State &state)
{
    CSDL2Types::coffee_sdl2_set_winflags(getSDL2Context()->window,state);
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