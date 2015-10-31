#include "csdl2renderer.h"

#include "coffeesdl2/sdl2helpers.h"
#include "coffeesdl2/sdl2eventhandlers.h"
#include "coffee/core/plat/plat_wm.h"
#include "coffee/core/base/cfiles.h"

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

std::thread::id CSDL2Renderer::contextThread()
{
    return m_context->thread;
}

void CSDL2Renderer::init(const CDWindowProperties &props)
{
    m_properties = props;

    m_context = new CSDL2Context;
    m_context->thread = std::this_thread::get_id();

//    cMsg("SDL2","Starting");

    bindingPreInit();

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC)<0)
    {
        cFatal("Failed to initialize SDL2 context: %s",SDL_GetError());
    }
    cMsg("SDL2","Initialized");

    CResources::CResource mapping("gamecontrollerdb.txt");
    if(mapping.exists())
    {
        cMsg("SDL2","Found game controller mappings");
        mapping.read_data(true);
        SDL_GameControllerAddMappingsFromRW(SDL_RWFromConstMem(mapping.data,mapping.size),0);
        mapping.free_data();
    }


    Uint32 flags = 0;
    flags = coffee_sdl2_interpret_winflags(props.flags);

    flags|=SDL_WINDOW_OPENGL;

    coffee_sdl2_set_context_properties(props.contextProperties);

    m_context->window =
            SDL_CreateWindow(props.title,
                             SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                             props.size.w,props.size.h,
                             flags);

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
//    cMsg("SDL2","VSync: %i",swapInterval());

//    cMsg("SDL2","Running %s",m_contextString.c_str());

//    cMsg("SDL2","OpenGL context created");

    bindingPostInit();

    for(int i=0;i<SDL_NumHaptics();i++)
    {
        SDL_Haptic* h = SDL_HapticOpen(i);
        SDL_HapticClose(h);
    }
    for(int i=0;i<SDL_NumJoysticks();i++)
    {
        SDL_Joystick* h = SDL_JoystickOpen(i);
        SDL_JoystickClose(h);
    }
}

void CSDL2Renderer::cleanup()
{
    if(m_context){
        bindingTerminate();
        cMsg("SDL2","Cleaning up context");

        for(std::pair<uint8,SDL_GameController*> con : m_context->controllers)
        {
            SDL_HapticClose(m_context->haptics[con.first]);
            SDL_GameControllerClose(m_context->controllers[con.first]);
        }

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

CDWindow* CSDL2Renderer::window()
{
    CDWindow* win = new CDWindow;

    SDL_GetWindowSize(m_context->window,&win->screenArea.w,&win->screenArea.h);
    SDL_GetWindowPosition(m_context->window,&win->screenArea.x,&win->screenArea.y);

    win->title = SDL_GetWindowTitle(m_context->window);
    coffee_sdl2_get_window_ptr(m_context->window,win);

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

bool CSDL2Renderer::screensaverMode()
{
    return SDL_IsScreenSaverEnabled();
}

void CSDL2Renderer::setScreensaverMode(bool state)
{
    if(state)
        SDL_EnableScreenSaver();
    else
        SDL_DisableScreenSaver();
}

CPoint CSDL2Renderer::windowPosition() const
{
    CPoint pos;
    SDL_GetWindowSize(m_context->window,&pos.x,&pos.y);
    return pos;
}

void CSDL2Renderer::setWindowPosition(const CPoint& pos)
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

int CSDL2Renderer::swapInterval() const
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

CPoint CSDL2Renderer::mousePosition() const
{
    CPoint p;
    SDL_GetMouseState(&p.x,&p.y);
    return p;
}

void CSDL2Renderer::setMousePosition(const CPoint &pos)
{
    SDL_WarpMouseInWindow(m_context->window,pos.x,pos.y);
}

bool CSDL2Renderer::relativeMouse() const
{
    return SDL_GetRelativeMouseMode();
}

void CSDL2Renderer::setRelativeMouse(bool enable)
{
    SDL_SetRelativeMouseMode((enable) ? SDL_TRUE : SDL_FALSE);
}

bool CSDL2Renderer::textInputMode() const
{
    return SDL_IsTextInputActive();
}

void CSDL2Renderer::setTextInputMode(bool state)
{
    if(state)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
}

void CSDL2Renderer::setTextArea(const CRect &area)
{
    SDL_Rect r;

    r.x = area.x;
    r.y = area.y;
    r.w = area.w;
    r.h = area.h;

    SDL_SetTextInputRect(&r);
}

void CSDL2Renderer::setKeyboardRepeat(bool m)
{
}

void CSDL2Renderer::eventHapticHandle(const CIHapticEvent *haptic)
{
    SDL_HapticRumblePlay(
                m_context->haptics[haptic->rumble_input.index],
            haptic->rumble_input.strength,
            haptic->rumble_input.duration);
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

/*!
 * \brief Opens and acquires a haptic device for a joystick
 * \param controller Controller for which we acquire a haptic device
 * \param index Index of controller used for associating it with a controller
 * \param dev Reference to a pointer to the haptic device
 * \return A valid pointer to a CIHapticEvent
 */
CIEvent* sdl2_controller_get_haptic(
        SDL_GameController* controller,
        uint8 index,
        SDL_Haptic*& dev)
{
    dev = nullptr;

    if(!controller)
        return nullptr;

    SDL_Joystick* js = SDL_GameControllerGetJoystick(controller);
    dev = SDL_HapticOpenFromJoystick(js);
    if(dev)
    {
        int idx = SDL_HapticIndex(dev);
        cstring hname = SDL_HapticName(idx);
        CIEvent* ev = (CIEvent*)calloc(1,sizeof(CIEvent)
                                       +sizeof(CIHapticEvent)
                                       +strlen(hname)-6);
        ev->type = CIEvent::HapticDev;
        CIHapticEvent *h = (CIHapticEvent*)&ev[1];
        memcpy((byte*)&h->rumble_device.name,
               hname,
               strlen(hname)+1);
        h->rumble_device.index = index;

        if(SDL_HapticRumbleInit(dev)==0)
            return ev;
        free(ev);
        SDL_HapticClose(dev);
    }
    return nullptr;
}

void CSDL2Renderer::_controllers_handle(const CIControllerAtomicUpdateEvent *ev)
{
    if(ev->connected){
        if(m_context->controllers[ev->controller])
            return;

        if(ev->remapped){
            cMsg("SDL2","Controller remapped: %i",ev->controller);
            SDL_GameControllerClose(m_context->controllers.at(ev->controller));
        }
        SDL_GameController* gc = SDL_GameControllerOpen(ev->controller);
        m_context->controllers[ev->controller] = gc;

        SDL_Haptic* hdev;
        CIEvent* hev = sdl2_controller_get_haptic(gc,ev->controller,hdev);
        if(hev)
        {
            cMsg("SDL2","Controller %i with rumble connected: %s",ev->controller,ev->name);
            m_context->haptics[ev->controller] = hdev;
            eventInputHandle(hev);
            free(hev);
        }else
            cMsg("SDL2","Controller %i connected: %s",ev->controller,ev->name);
    }else{
        if(!m_context->controllers[ev->controller])
            return;

        SDL_GameControllerClose(m_context->controllers[ev->controller]);
        m_context->controllers.erase(ev->controller);
        SDL_HapticClose(m_context->haptics[ev->controller]);
        m_context->haptics.erase(ev->controller);
        cMsg("SDL2","Controller %i disconnected",ev->controller);
    }
}

}
}
