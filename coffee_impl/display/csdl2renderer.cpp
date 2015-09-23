#include "csdl2renderer.h"

#include "coffeesdl2/sdl2helpers.h"
#include "coffeesdl2/sdl2eventhandlers.h"

namespace Coffee{
namespace CDisplay{

using namespace Coffee::CSDL2Types;

CSDL2Renderer::CSDL2Renderer(Coffee::CObject *parent) :
    CDQueueRendererBase(parent)
{

}

CSDL2Renderer::~CSDL2Renderer()
{
    cleanup();
}

thread_id CSDL2Renderer::contextThread()
{
    return m_context->thread;
}

void CSDL2Renderer::init(const CDWindowProperties &props)
{
    m_properties = props;

    m_context = new CSDL2Context;
    m_context->thread = std::this_thread::get_id();

    cMsg("SDL2","Starting");

    bindingPreInit();

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC)<0)
    {
        cFatal("Failed to initialize SDL2 context: %s",SDL_GetError());
    }
    cMsg("SDL2","Initialized");

    Uint32 flags = coffee_sdl2_interpret_winflags(props.flags);

    flags|=SDL_WINDOW_OPENGL;

    coffee_sdl2_set_context_properties(props.contextProperties);

    m_context->window =
            SDL_CreateWindow(props.title,
                             SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                             props.size.w,props.size.h,flags);

    if(!m_context->window){
        cFatal("Failed to create SDL2 window: %s",SDL_GetError());
    }

    m_context->context = SDL_GL_CreateContext(m_context->window);

    if(SDL_GL_MakeCurrent(m_context->window,m_context->context))
    {
        cFatal("Failed to create SDL2 OpenGL context: %s",SDL_GetError());
    }

    SDL_version ver;
    SDL_GetVersion(&ver);
    m_contextString = cStringFormat("SDL %i.%i.%i",ver.major,ver.minor,ver.patch);

    //For consistent behavior
    if(props.contextProperties.flags&CGLContextProperties::GLVSync)
        setSwapInterval(1);
    else
        setSwapInterval(0);

    if(props.flags&CDWindowProperties::Windowed)
        SDL_SetWindowFullscreen(m_context->window,0);

    setMouseGrabbing(false);

    setSwapInterval(0);
    cMsg("SDL2","VSync: %i",swapInterval());

    cMsg("SDL2","Running %s",m_contextString.c_str());

    cMsg("SDL2","OpenGL context created");

    bindingPostInit();
}

void CSDL2Renderer::cleanup()
{
    if(m_context){
        bindingTerminate();
        cMsg("SDL2","Cleaning up context");
        SDL_GL_DeleteContext(m_context->context);
        SDL_DestroyWindow(m_context->window);
        SDL_Quit();
        delete m_context;
        m_context = nullptr;
        cMsg("SDL2","Terminated");
    }else{
        cMsg("SDL2","Already cleaned up");
    }
}

CString CSDL2Renderer::windowTitle() const
{
    return SDL_GetWindowTitle(m_context->window);
}

void CSDL2Renderer::setWindowTitle(const CString &title)
{
    SDL_SetWindowTitle(m_context->window,title.c_str());
}

double CSDL2Renderer::contextTime()
{
    return (double)SDL_GetTicks()/1000.0;
}

CDMonitor CSDL2Renderer::monitor()
{
    CDMonitor mon;

    SDL_DisplayMode dm;
    if(SDL_GetCurrentDisplayMode(m_properties.monitor,&dm)<0)
        cDebug("Failed to get monitor information: %s",SDL_GetError());
    else{

        mon.refresh = dm.refresh_rate;
        mon.screenArea.w = dm.w;
        mon.screenArea.h = dm.h;
        mon.name = SDL_GetDisplayName(m_properties.monitor);
        mon.index = m_properties.monitor;

        //TODO: Add color space
        //TODO: Add physical dimensions
        //TODO: Add screenArea offsets
    }

    return mon;
}

CDWindow CSDL2Renderer::window()
{
    CDWindow win;

    SDL_GetWindowSize(m_context->window,&win.screenArea.w,&win.screenArea.h);
    SDL_GetWindowPosition(m_context->window,&win.screenArea.x,&win.screenArea.y);

    win.title = SDL_GetWindowTitle(m_context->window);
    coffee_sdl2_get_window_ptr(m_context->window,&win);

    return win;
}

CDContextBits CSDL2Renderer::context()
{
    return coffee_sdl2_get_context_properties().bits;
}

uint32 CSDL2Renderer::windowState() const
{
    return coffee_sdl2_get_winflags(m_context->window);
}

void CSDL2Renderer::setWindowState(uint32 state)
{
    coffee_sdl2_set_winflags(m_context->window,state);
}

CPoint CSDL2Renderer::windowPosition() const
{
    CPoint pos;
    SDL_GetWindowSize(m_context->window,&pos.x,&pos.y);
    return pos;
}

void CSDL2Renderer::setWindowPosition(CPoint pos)
{
    SDL_SetWindowPosition(m_context->window,pos.x,pos.y);
}

bool CSDL2Renderer::showWindow()
{
    SDL_ShowWindow(m_context->window);
    return true;
}

bool CSDL2Renderer::hideWindow()
{
    SDL_HideWindow(m_context->window);
    return true;
}

bool CSDL2Renderer::closeWindow()
{
    m_context->contextFlags |= 0b1;
    return true;
}

bool CSDL2Renderer::closeFlag()
{
    return m_context->contextFlags&0b1;
}

int CSDL2Renderer::swapInterval()
{
    return SDL_GL_GetSwapInterval();
}

void CSDL2Renderer::setSwapInterval(int interval)
{
    SDL_GL_SetSwapInterval(interval);
}

CSize CSDL2Renderer::framebufferSize() const
{
    CSize sz;
    SDL_GL_GetDrawableSize(m_context->window,&sz.w,&sz.h);
    cDebug("Framebuffer size: %ix%i",sz.w,sz.h);
    return sz;
}

CSize CSDL2Renderer::windowSize() const
{
    CSize size;
    SDL_GetWindowSize(m_context->window,&size.w,&size.h);
    return size;
}

void CSDL2Renderer::setWindowSize(const CSize &size)
{
    SDL_SetWindowSize(m_context->window,size.w,size.h);
    SDL_SetWindowMinimumSize(m_context->window,size.w,size.h);
    SDL_SetWindowMaximumSize(m_context->window,size.w,size.h);
}

bool CSDL2Renderer::isMouseGrabbed() const
{
    return (SDL_GetWindowGrab(m_context->window)==SDL_TRUE);
}

void CSDL2Renderer::setMouseGrabbing(bool grab)
{
    SDL_SetWindowGrab(m_context->window,(grab) ? SDL_TRUE : SDL_FALSE);
}

void CSDL2Renderer::swapBuffers()
{
    SDL_GL_SwapWindow(m_context->window);
}

void CSDL2Renderer::pollEvents()
{
    while(SDL_PollEvent(&m_context->eventhandle))
        coffee_sdl2_eventhandle_all(this,&m_context->eventhandle);
}

}
}
