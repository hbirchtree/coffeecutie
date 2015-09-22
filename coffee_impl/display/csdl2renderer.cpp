#include "csdl2renderer.h"

#include "coffeesdl2/sdl2datatypes.h"

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
    delete m_context;
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

    cMsg("SDL2","OpenGL context created");

    bindingPostInit();
}

void CSDL2Renderer::cleanup()
{
    if(m_context){
        cMsg("SDL2","Cleaning up context");
        SDL_GL_DeleteContext(m_context->context);
        SDL_DestroyWindow(m_context->window);
        SDL_Quit();
        delete m_context;
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
    return 0.0;
}

CDMonitor CSDL2Renderer::monitor()
{

}

CDWindow CSDL2Renderer::window()
{

}

CDContextBits CSDL2Renderer::context()
{

}

uint32_t CSDL2Renderer::windowState() const
{

}

void CSDL2Renderer::setWindowState(uint32_t)
{

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
}

bool CSDL2Renderer::hideWindow()
{
    SDL_HideWindow(m_context->window);
}

bool CSDL2Renderer::closeWindow()
{

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
    return CSize(-1,-1);
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
    if(SDL_PollEvent(&m_context->eventhandle)){
        if(m_context->eventhandle.type == SDL_WINDOWEVENT){
            if(m_context->eventhandle.window.type == SDL_WINDOWEVENT_CLOSE)
                m_context->contextFlags |= 0b1;
        }


    }
}

bool CSDL2Renderer::requestGLExtension(cstring)
{
    return false;
}

}
}
