#include <coffee/sdl2/csdl2renderer.h>

#include "sdl2helpers.h"
#include "sdl2eventhandlers.h"
#include <coffee/core/base/cfiles.h>

namespace Coffee{
namespace CDisplay{

using namespace Coffee::CSDL2Types;

struct CGL_SDL_GL_Context : CGL::CGL_Context
{
    CGL_SDL_GL_Context(SDL_Window* win):
        m_window(win)
    {
        m_context = SDL_GL_CreateContext(win);
        acquireContext();
    }
    ~CGL_SDL_GL_Context()
    {
        releaseContext();
        SDL_GL_DeleteContext(m_context);
    }
    bool acquireContext()
    {
        new (&m_threadId) ThreadId_t;
        return SDL_GL_MakeCurrent(m_window,m_context)==0;
    }
    bool releaseContext()
    {
        return SDL_GL_MakeCurrent(nullptr,nullptr)==0;
    }
    const ThreadId_t &currentThread()
    {
        return m_threadId;
    }
protected:
    ThreadId_t m_threadId;
private:
    SDL_Window* m_window;
    SDL_GLContext m_context;
};

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

void CSDL2Renderer::init(const CDProperties &props)
{
    Profiler::PushContext("SDL2");

    m_properties = props;

    /* Allocate context objects, will hold SDL-specific state so that SDL won't litter the external namespace */
    m_context = new CSDL2Context;
    m_context->thread = std::this_thread::get_id();

    /* Run pre-init for GL binding, for modularity reasons */
    bindingPreInit();

    /* Get current SDL version */
    SDL_version ver;
    SDL_GetVersion(&ver);
    m_contextString = cStringFormat("SDL {0}.{1}.{2}",ver.major,ver.minor,ver.patch);

    Profiler::Profile();

    /* Initialize SDL and its components */
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC)<0)
    {
        cFatal("Failed to initialize SDL2 context: {0}",SDL_GetError());
    }

    Profiler::Profile();

    cMsg("SDL2","Initialized");

    /* If found, load game controller mappings from file */
    CResources::CResource mapping("gamecontrollerdb.txt");
    if(FileExists(mapping))
    {
        cMsg("SDL2","Found game controller mappings");
        FilePull(mapping,true);
        SDL_RWops* fsrc = SDL_RWFromConstMem(mapping.data,mapping.size);
        SDL_GameControllerAddMappingsFromRW(fsrc,0);
        SDL_FreeRW(fsrc);
        FileFree(mapping);
    }

    Profiler::Profile();

    /* Translate window flags and apply them */
    Uint32 flags = 0;
    flags = coffee_sdl2_interpret_winflags(props.flags);

    /* Special flag for SDL to enable OpenGL context */
    flags|=SDL_WINDOW_OPENGL;

    coffee_sdl2_set_context_properties(props.gl);

    /* Create the platform window */
    m_context->window =
            SDL_CreateWindow(props.title,
                             SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                             props.size.w,props.size.h,
                             flags);

    Profiler::Profile();

    /* Validate the window pointer, may have failed */
    if(!m_context->window){
        cFatal("Failed to create SDL2 window: {0}",SDL_GetError());
    }

    /* If an icon pointer is present, use it */
    if(m_properties.icon)
    {
        setWindowIcon(*m_properties.icon);
    }

    /* Show window if requested */
    if(m_properties.flags&CDProperties::Visible)
        showWindow();

    /* Acquire the OpenGL context */
    m_context->context = new CGL_SDL_GL_Context(m_context->window);

    Profiler::Profile();

    /* Make the GL context current to this thread */
    if(!m_context->context->acquireContext())
    {
        CString err = cStringFormat("Failed to create SDL2 OpenGL context:\n{0}",SDL_GetError());
        this->popErrorMessage(Severity::Fatal,"GL init failure",err.c_str());
        cFatal("{0}",err.c_str());
    }

    Profiler::Profile();

    /* Enable VSync if requested */
    if(props.gl.flags&GLProperties::GLVSync)
        setSwapInterval(1);
    else
        setSwapInterval(0);

    /* Set windowed fullscreen if requested */
    if(props.flags&CDProperties::Windowed)
        SDL_SetWindowFullscreen(m_context->window,0);

    /* Disable mouse grabbing in the start, up to the user to enable it */
    setMouseGrabbing(false);

    cMsg("SDL2","Running {0}",m_contextString);

    Profiler::Profile();

    /* Run binding post-init, fetches GL extensions and etc. */
    bindingPostInit();

    Profiler::Profile();

    /* WORKAROUND: Sometimes, haptic and joystick devices are not closed properly. This part ensures that they are indeed closed before starting. Everything else should be fine. */
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

    Profiler::PopContext();
}

void CSDL2Renderer::cleanup()
{
    if(m_context){
        /* Acquire GL context first, ensures that destructor won't fail on different thread */
        m_context->context->acquireContext();

        /* Run binding termination if needed, should clean up any allocated data */
        bindingTerminate();

        cMsg("SDL2","Cleaning up context");

        /* Close haptic devices and game controllers */
        for(std::pair<uint8,SDL_GameController*> con : m_context->controllers)
        {
            SDL_HapticClose(m_context->haptics[con.first]);
            SDL_GameControllerClose(m_context->controllers[con.first]);
        }

        /* Delete GL context object */
        delete m_context->context;
        /* Delete window */
        SDL_DestroyWindow(m_context->window);
        /* De-initialize SDL */
        SDL_Quit();
        delete m_context;
        m_context = nullptr;
        cMsg("SDL2","Terminated");
    }else{
        /* This happens if cleanup has happened before destruction, or if cleanup is called multiple times. Either way is fine. */
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
        cDebug("Failed to get monitor information: {0}",SDL_GetError());
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
    return coffee_sdl2_get_window(m_context->window);
}

CDContextBits CSDL2Renderer::context()
{
    return coffee_sdl2_get_context_properties().bits;
}

uint32 CSDL2Renderer::windowState() const
{
    return coffee_sdl2_get_winflags(m_context->window);
}

void CSDL2Renderer::setWindowState(const CDProperties::State &state)
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
    m_context->contextFlags |= 0x1;
    return true;
}

bool CSDL2Renderer::closeFlag()
{
    return m_context->contextFlags&0x1;
}

int CSDL2Renderer::swapInterval() const
{
    return SDL_GL_GetSwapInterval();
}

void CSDL2Renderer::setSwapInterval(const int &interval)
{
    SDL_GL_SetSwapInterval(interval);
}

CSize CSDL2Renderer::framebufferSize() const
{
    CSize sz;
    SDL_GL_GetDrawableSize(m_context->window,&sz.w,&sz.h);
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

void CSDL2Renderer::eventHandleH(const CIHapticEvent &haptic, c_cptr)
{
    SDL_HapticRumblePlay(
                m_context->haptics[haptic.rumble_input.index],
            haptic.rumble_input.strength,
            haptic.rumble_input.duration);
}

/*!
 * \brief A default event handler created to intercept controller connect and disconnect. This is optional, and the user may choose to implement their own methods based on this.
 * \param event
 */
void CSDL2Renderer::eventHandleI(const CIEvent &event, c_cptr data)
{
    if(event.type==CIEvent::ControllerUpdate)
    {
        const CIControllerAtomicUpdateEvent* jev =
                (const CIControllerAtomicUpdateEvent*)data;
        _sdl2_controllers_handle(jev);
    }
}

void CSDL2Renderer::eventHandleD(const CDEvent &event, c_cptr data)
{
    if(event.type==CDEvent::State)
    {
        const CDStateEvent* sev = (const CDStateEvent*)data;
        if(sev->type==CDStateEvent::Closed)
            this->closeWindow();
    }
}

CIControllerState CSDL2Renderer::getControllerState(uint16 index)
{
    SDL_GameController* gc = m_context->controllers[index];
    CIControllerState state;

    state.axes.e.l_x = SDL_GameControllerGetAxis(gc,SDL_CONTROLLER_AXIS_LEFTX);
    state.axes.e.l_y = SDL_GameControllerGetAxis(gc,SDL_CONTROLLER_AXIS_LEFTY);

    state.axes.e.r_x = SDL_GameControllerGetAxis(gc,SDL_CONTROLLER_AXIS_RIGHTX);
    state.axes.e.r_x = SDL_GameControllerGetAxis(gc,SDL_CONTROLLER_AXIS_RIGHTY);

    state.axes.e.t_l = SDL_GameControllerGetAxis(gc,SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    state.axes.e.t_r = SDL_GameControllerGetAxis(gc,SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

    state.buttons.e.a = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_A);
    state.buttons.e.b = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_B);
    state.buttons.e.x = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_X);
    state.buttons.e.y = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_Y);

    state.buttons.e.back = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_BACK);
    state.buttons.e.guide = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_GUIDE);
    state.buttons.e.start = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_START);

    state.buttons.e.s_l = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_LEFTSTICK);
    state.buttons.e.s_r = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_RIGHTSTICK);

    state.buttons.e.b_l = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    state.buttons.e.b_r = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

    state.buttons.e.p_up = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_DPAD_UP);
    state.buttons.e.p_down = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    state.buttons.e.p_left = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    state.buttons.e.p_right = SDL_GameControllerGetButton(gc,SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    return state;
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

CGL::CGL_Context *CSDL2Renderer::glContext()
{
    return m_context->context;
}

CGL::CGL_ScopedContext CSDL2Renderer::scopedContext()
{
    return CGL::CGL_ScopedContext(m_context->context);
}

bool CSDL2Renderer::setWindowIcon(CBitmap &icon)
{

    SDL_Surface* sdl_icon = SDL_CreateRGBSurfaceFrom(icon.data(),
                                                     icon.size.w,icon.size.h,
                                                     32,sizeof(CRGBA),
                                                     0,0,0,0);
    if(!sdl_icon)
        return false;
    SDL_SetWindowIcon(m_context->window,sdl_icon);
    SDL_FreeSurface(sdl_icon);
    return true;
}

void CSDL2Renderer::popErrorMessage(Severity s, cstring title, cstring msg)
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
        CIEvent* ev = (CIEvent*)CCalloc(1,sizeof(CIEvent)
                                       +sizeof(CIHapticEvent)
                                       +strlen(hname)-6);
        ev->type = CIEvent::HapticDev;
        CIHapticEvent *h = (CIHapticEvent*)&ev[1];
        CMemCpy((byte_t*)&h->rumble_device.name,
               hname,
               strlen(hname)+1);
        h->rumble_device.index = index;

        if(SDL_HapticRumbleInit(dev)==0)
            return ev;
        CFree(ev);
        SDL_HapticClose(dev);
    }
    return nullptr;
}

void CSDL2Renderer::_sdl2_controllers_handle(const CIControllerAtomicUpdateEvent *ev)
{
    if(ev->connected){
        if(m_context->controllers[ev->controller])
            return;

        if(ev->remapped){
            cMsg("SDL2","Controller remapped: {0}",ev->controller);
            SDL_GameControllerClose(m_context->controllers.at(ev->controller));
        }
        SDL_GameController* gc = SDL_GameControllerOpen(ev->controller);
        m_context->controllers[ev->controller] = gc;

        SDL_Haptic* hdev;
        CIEvent* hev = sdl2_controller_get_haptic(gc,ev->controller,hdev);
        if(hev)
        {
            cMsg("SDL2","Controller {0} with rumble connected: {1}",
                 ev->controller,
                 ev->name);

            m_context->haptics[ev->controller] = hdev;
            eventHandle(*hev,nullptr);
            CFree(hev);
        }else
            cMsg("SDL2","Controller {0} connected: {1}",ev->controller,ev->name);
    }else{
        if(!m_context->controllers[ev->controller])
            return;

        SDL_GameControllerClose(m_context->controllers[ev->controller]);
        m_context->controllers.erase(ev->controller);
        SDL_HapticClose(m_context->haptics[ev->controller]);
        m_context->haptics.erase(ev->controller);
        cMsg("SDL2","Controller {0} disconnected",ev->controller);
    }
}

void SDL2Dialog::ErrorMessage(cstring title, cstring message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title,message,NULL);
}

void SDL2Dialog::WarningMessage(cstring title, cstring message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,title,message,NULL);
}

void SDL2Dialog::InformationMessage(cstring title, cstring message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,title,message,NULL);
}

}
}
