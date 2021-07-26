#include <coffee/sdl2_comp/sdl2_components.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/entity_container.inl>
#include <coffee/core/CProfiling>
#include <peripherals/stl/string_casting.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include "sdl2events.h"

#include <SDL.h>

#define NOT_ZERO(v) (v < 0)

#define EMIT_IEVENT(from)                  \
    {                                      \
        auto data    = from;               \
        inputEv.type = data.event_type;    \
        inputBus->process(inputEv, &data); \
    }

namespace sdl2 {

using F      = comp_app::detail::WindowState;
using WState = comp_app::detail::WindowState;

static constexpr stl_types::Array<stl_types::Pair<Uint32, F>, 7>
    window_flag_mapping = {
        {{SDL_WINDOW_SHOWN, F::Visible},

         {SDL_WINDOW_MINIMIZED, F::Minimized},
         {SDL_WINDOW_MAXIMIZED, F::Maximized},
         {SDL_WINDOW_RESIZABLE, F::Resizable},
         {SDL_WINDOW_BORDERLESS, F::Undecorated},

         {SDL_WINDOW_FULLSCREEN, F::FullScreen},
         {SDL_WINDOW_FULLSCREEN_DESKTOP, F::WindowedFullScreen}}};

static ::Uint32 window_to_sdl2(comp_app::detail::WindowState state)
{
    Uint32 out = 0;

    for(auto const& v : window_flag_mapping)
        if(state & v.second)
            out |= v.first;

    return out;
}

static libc_types::u16 window_from_sdl2(::Uint32 state)
{
    libc_types::u16 out = 0;

    for(auto const& v : window_flag_mapping)
        if(state & v.first)
            out |= v.second;

    return out;
}

static void getWindow(
    SDL_Window* window, comp_app::interfaces::PtrNativeWindowInfo& info);

using namespace stl_types;

void Context::load(entity_container& c, comp_app::app_error&)
{
    C_EXPECT_ZERO(SDL_Init(0))

    c.register_subsystem_services<comp_app::AppServiceTraits<Context>>(this);

    if(auto info = c.service<comp_app::AppInfo>(); info)
    {
        SDL_version ver;
        SDL_GetVersion(&ver);

        auto verString = cast_pod(ver.major) + "." + cast_pod(ver.minor) + "." +
                         cast_pod(ver.patch);

        info->add("sdl2:version", verString);
    }
}

void Context::unload(entity_container&, comp_app::app_error&)
{
    SDL_Quit();
}

void Context::end_restricted(proxy_type& p, time_point const&)
{
    using namespace Coffee::Display;
    using namespace Coffee::Input;

    auto inputBus = p.service<comp_app::EventBus<CIEvent>>();

    C_PTR_CHECK_MSG(inputBus, "EventBus<CIEvent> not specified")

    CIEvent inputEv;

    SDL_Event event;
    while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUIT, SDL_QUIT))
    {
        auto data    = translate::event<CIQuit>(event);
        inputEv.type = data.event_type;
        inputBus->process(inputEv, &data);

        m_shouldClose = data.shouldClose;
    }
}

Windowing::~Windowing()
{
}

void Windowing::load(entity_container& c, comp_app::app_error& ec)
{
#if defined(COFFEE_LINUX) || defined(COFFEE_EMSCRIPTEN)
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
#endif

    if(NOT_ZERO(SDL_InitSubSystem(SDL_INIT_VIDEO)))
    {
        ec = comp_app::AppError::NoDisplay;
        ec = SDL_GetError();
        return;
    }

    auto& config = comp_app::AppLoader::config<comp_app::WindowConfig>(c);

    Uint32 extraFlags = SDL_WINDOW_ALLOW_HIGHDPI;

    auto glContext = c.service<GLContext>();
    if(glContext)
    {
        extraFlags |= SDL_WINDOW_OPENGL;
        glContext->setupAttributes(c);
    } else
        extraFlags |= SDL_WINDOW_VULKAN;

    m_window = SDL_CreateWindow(
        config.title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        config.size.w,
        config.size.h,
        sdl2::window_to_sdl2(config.flags) | extraFlags);

    if(!m_window)
    {
        ec = comp_app::AppError::NoWindow;
        ec = SDL_GetError();
        return;
    }

    SDL_SetWindowTitle(m_window, config.title.c_str());

    auto nativeWindowInfo = c.service<comp_app::PtrNativeWindowInfo>();
    if(nativeWindowInfo)
        getWindow(m_window, *nativeWindowInfo);

    m_container = &c;
}

void Windowing::unload(entity_container&, comp_app::app_error&)
{
    if(m_window)
        SDL_DestroyWindow(m_window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Windowing::start_restricted(proxy_type& p, time_point const&)
{
    using namespace Coffee::Display;

    auto  displayBus = get_container(p).service<comp_app::EventBus<Event>>();
    Event displayEv;

#define EMIT_DEVENT(from)                      \
    {                                          \
        auto data      = from;                 \
        displayEv.type = data.event_type;      \
        displayBus->process(displayEv, &data); \
    }

    SDL_Event event;
    SDL_PumpEvents();

    while(SDL_PeepEvents(
        &event,
        1,
        SDL_eventaction::SDL_GETEVENT,
        SDL_WINDOWEVENT,
        SDL_WINDOWEVENT))
    {
        switch(event.window.event)
        {
        case SDL_WINDOWEVENT_ENTER:
        case SDL_WINDOWEVENT_LEAVE:
        case SDL_WINDOWEVENT_EXPOSED:
        case SDL_WINDOWEVENT_FOCUS_LOST:
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            EMIT_DEVENT(translate::event<FocusEvent>(event))
            break;
        case SDL_WINDOWEVENT_RESIZED:
            EMIT_DEVENT(translate::event<ResizeEvent>(event))
            break;
        case SDL_WINDOWEVENT_MOVED:
            EMIT_DEVENT(translate::event<MoveEvent>(event))
            break;
        default:
            break;
        }
    }

#undef EMIT_DEVENT
}

void Windowing::show()
{
    SDL_ShowWindow(m_window);
}

void Windowing::close()
{
    SDL_HideWindow(m_window);
    m_container->service<Context>()->m_shouldClose = true;
}

comp_app::size_2d_t Windowing::size() const
{
    size_2d_t out;
    SDL_GetWindowSize(m_window, &out.w, &out.h);
    return out;
}

void Windowing::resize(const comp_app::size_2d_t& newSize)
{
    SDL_SetWindowSize(m_window, newSize.w, newSize.h);
}

comp_app::position_t Windowing::position() const
{
    position_t out;
    SDL_GetWindowPosition(m_window, &out.x, &out.y);
    return out;
}

void Windowing::move(const comp_app::position_t& newPos)
{
    SDL_SetWindowPosition(m_window, newPos.x, newPos.y);
}

comp_app::detail::WindowState Windowing::state() const
{
    return static_cast<WState>(window_from_sdl2(SDL_GetWindowFlags(m_window)));
}

void Windowing::setState(comp_app::detail::WindowState state)
{
    if(state & F::Visible)
        SDL_ShowWindow(m_window);

    if(state & F::Minimized)
        SDL_MinimizeWindow(m_window);
    if(state & F::Maximized)
        SDL_MaximizeWindow(m_window);

    if(state & F::FullScreen)
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    if(state & F::WindowedFullScreen)
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(state & F::Normal)
        SDL_SetWindowFullscreen(m_window, 0);

    if(state & F::Undecorated)
        SDL_SetWindowBordered(m_window, SDL_FALSE);
    else
        SDL_SetWindowBordered(m_window, SDL_TRUE);

    if(state & F::Focused)
        SDL_RaiseWindow(m_window);
}

bool Windowing::notifiedClose() const
{
    return m_container->service<Context>()->m_shouldClose;
}

comp_app::size_2d_t DisplayInfo::virtualSize() const
{
    size_2d_t out;
    SDL_Rect  displayRect = {};
    for(auto i : Range<int>(SDL_GetNumVideoDisplays()))
    {
        SDL_GetDisplayBounds(i, &displayRect);
        out.w = std::max(out.w, displayRect.x + displayRect.w);
        out.h = std::max(out.h, displayRect.y + displayRect.h);
    }
    return out;
}

libc_types::u32 DisplayInfo::count() const
{
    return C_FCAST<libc_types::u32>(SDL_GetNumVideoDisplays());
}

libc_types::u32 DisplayInfo::currentDisplay() const
{
    return C_FCAST<libc_types::u32>(SDL_GetWindowDisplayIndex(nullptr));
}

comp_app::size_2d_t DisplayInfo::size(libc_types::u32 idx) const
{
    SDL_Rect rect;
    SDL_GetDisplayBounds(C_FCAST<libc_types::i32>(idx), &rect);
    return {rect.w, rect.h};
}

comp_app::size_2d_t DisplayInfo::physicalSize(libc_types::u32) const
{
    return {};
}

void GLContext::setupAttributes(entity_container& c)
{
    using GLConfig = comp_app::GLConfig;
    auto& glConfig = comp_app::AppLoader::config<GLConfig>(c);

    using namespace typing::pixels;

    SDL_GL_SetAttribute(
        SDL_GL_FRAMEBUFFER_SRGB_CAPABLE,
        properties::get<properties::supports_srgb>(glConfig.framebufferFmt)
            ? SDL_TRUE
            : SDL_FALSE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glConfig.version.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glConfig.version.minor);

    {
        Sint32 contextProfile = SDL_GL_CONTEXT_PROFILE_CORE;

        if(glConfig.profile & GLConfig::Embedded)
            contextProfile = SDL_GL_CONTEXT_PROFILE_ES;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, contextProfile);
    }

    {
        Sint32 contextFlags = 0;

        if constexpr(compile_info::platform::is_macos)
            contextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

        if(glConfig.profile & GLConfig::Robust)
            contextFlags |= SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
        if(glConfig.profile & GLConfig::Debug)
            contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
    }

    using namespace typing::pixels;

    auto color = properties::get<properties::layout>(glConfig.framebufferFmt);
    auto depth_stencil = properties::get<properties::layout>(glConfig.depthFmt);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, color.r);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, color.g);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, color.b);

    if(glConfig.multisampling.enabled)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, SDL_TRUE);
        SDL_GL_SetAttribute(
            SDL_GL_MULTISAMPLESAMPLES, glConfig.multisampling.samples);
    }

    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, color.a);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth_stencil.depth);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, depth_stencil.stencil);
}

struct try_create_context
{
    try_create_context(
        SDL_Window*                 window,
        SDL_GLContext*              context,
        comp_app::GLConfig::Profile profile) :
        window(window),
        context(context), profile(profile)
    {
    }

    template<typename version>
    void operator()()
    {
        if((profile & version::profile) == 0 || *context)
            return;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version::major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version::minor);

        *context = SDL_GL_CreateContext(window);

        if(*context)
            SDL_GetError();
    }

    SDL_Window*                 window;
    SDL_GLContext*              context;
    comp_app::GLConfig::Profile profile;
};

void GLContext::load(entity_container& c, comp_app::app_error& ec)
{
    using comp_app::GLConfig;

    auto  window   = c.service<Windowing>()->m_window;
    auto& glConfig = comp_app::AppLoader::config<GLConfig>(c);

    m_context = nullptr;

#if defined(COFFEE_EMSCRIPTEN)
    auto try_create = try_create_context(window, &m_context, glConfig.profile);
    try_create.template operator()<GLConfig::gles3_version>();
    try_create.template operator()<GLConfig::gles2_version>();
#else
    type_list::for_each_rev<GLConfig::valid_versions>(
        try_create_context(window, &m_context, glConfig.profile));
#endif

    if(!m_context)
    {
        ec = SDL_GetError();
        ec = comp_app::AppError::NoContext;
        return;
    }

    if(SDL_GL_MakeCurrent(window, m_context) != 0)
    {
        ec = SDL_GetError();
        ec = comp_app::AppError::SystemError;
        return;
    }

#if !defined(COFFEE_EMSCRIPTEN)
    c.service<GLSwapControl>()->setSwapInterval(glConfig.swapInterval);
#endif

    {
        auto& bindConf =
            comp_app::AppLoader::config<comp_app::GraphicsBindingConfig>(c);
        bindConf.loader = SDL_GL_GetProcAddress;
    }

    m_container = &c;
}

void GLContext::unload(entity_container&, comp_app::app_error&)
{
    if(m_context)
        SDL_GL_DeleteContext(m_context);
}

libc_types::i32 GLSwapControl::swapInterval() const
{
    return SDL_GL_GetSwapInterval();
}

void GLSwapControl::setSwapInterval(libc_types::i32 interval)
{
    SDL_GL_SetSwapInterval(interval);
}

void GLFramebuffer::load(entity_container& c, comp_app::app_error&)
{
    m_container = &c;
}

void GLFramebuffer::swapBuffers(comp_app::app_error&)
{
    if constexpr(compile_info::debug_mode)
        Coffee::Profiler::PushContext("sdl2::GLFramebuffer::swapBuffers");

    SDL_GL_SwapWindow(m_container->service<sdl2::Windowing>()->m_window);

    if constexpr(compile_info::debug_mode)
        Coffee::Profiler::PopContext();
}

comp_app::size_2d_t GLFramebuffer::size() const
{
    size_2d_t out;
    SDL_GL_GetDrawableSize(
        m_container->service<Windowing>()->m_window, &out.w, &out.h);
    return out;
}

void GLFramebuffer::start_frame(
    comp_app::detail::ContainerProxy&, time_point const&)
{
    defaultSwap();
}

void ControllerInput::load(entity_container& c, comp_app::app_error& ec)
{
    if constexpr(compile_info::platform::is_emscripten)
        return;

    auto& config = comp_app::AppLoader::config<comp_app::ControllerConfig>(c);

    if(!config.mapping.empty())
        SDL_SetHint(SDL_HINT_GAMECONTROLLERCONFIG_FILE, "controllerdb.txt");
    if(config.options & comp_app::ControllerConfig::BackgroundInput)
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if(SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0)
    {
        ec = SDL_GetError();
        ec = comp_app::AppError::SystemError;
        return;
    }

    {
        constexpr auto max_val  = std::numeric_limits<libc_types::i16>::max();
        auto           dead_val = max_val - config.deadzone;

        m_axisScale    = max_val / libc_types::scalar(dead_val);
        m_axisDeadzone = config.deadzone;
    }
}

void ControllerInput::unload(entity_container&, comp_app::app_error&)
{
    if constexpr(compile_info::platform::is_emscripten)
        return;

    //    for(auto const& controller : m_controllers)
    //        SDL_GameControllerClose(
    //            C_RCAST<SDL_GameController*>(controller.second));

    for(auto i : stl_types::Range<int>(SDL_NumJoysticks()))
    {
        SDL_JoystickClose(SDL_JoystickOpen(i));
    }

    SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    SDL_GetError();
}

void ControllerInput::start_restricted(proxy_type& p, time_point const&)
{
    if constexpr(compile_info::platform::is_emscripten)
        return;

    using namespace Coffee::Input;

    auto    inputBus = get_container(p).service<comp_app::EventBus<CIEvent>>();
    CIEvent inputEv;

    SDL_Event event;
    SDL_PumpEvents();

    while(SDL_PeepEvents(
        &event,
        1,
        SDL_GETEVENT,
        SDL_CONTROLLERDEVICEADDED,
        SDL_CONTROLLERDEVICEREMAPPED))
    {
        if(event.type == SDL_CONTROLLERDEVICEADDED)
        {
            auto controller = SDL_GameControllerOpen(event.cdevice.which);

            if(!controller)
            {
                Throw(undefined_behavior("failed to open controller device"));
            }

            auto joystick = SDL_GameControllerGetJoystick(controller);

#if SDL_MAJOR_VERSION >= 2 && SDL_MINOR_VERSION >= 0 && SDL_PATCHLEVEL >= 8
            auto playerIdx = SDL_GameControllerGetPlayerIndex(controller);
#else
            auto playerIdx = -1;
#endif
            auto instanceId = SDL_JoystickInstanceID(joystick);

            playerIdx = playerIdx >= 0
                            ? playerIdx
                            : C_FCAST<libc_types::i32>(m_playerIndex.size());

            m_playerIndex.insert({playerIdx, controller});
            m_controllers.insert({instanceId, controller});

#if SDL_MAJOR_VERSION >= 2 && SDL_MINOR_VERSION >= 0 && SDL_PATCHLEVEL >= 8
            SDL_GameControllerRumble(controller, 7000, 9000, 200);
#endif

            event.cdevice.which = playerIdx;
        } else if(event.type == SDL_CONTROLLERDEVICEREMOVED)
        {
            event.cdevice.which = controllerDisconnect(event.cdevice.which);
        }

        EMIT_IEVENT(translate::event<CIControllerAtomicUpdateEvent>(event))
    }

    while(SDL_PeepEvents(
        &event,
        1,
        SDL_GETEVENT,
        SDL_CONTROLLERAXISMOTION,
        SDL_CONTROLLERBUTTONUP))
        ;

    while(SDL_PeepEvents(
        &event, 1, SDL_GETEVENT, SDL_JOYAXISMOTION, SDL_JOYDEVICEREMOVED))
        ;
}

libc_types::u32 ControllerInput::count() const
{
    return C_FCAST<libc_types::u32>(m_controllers.size());
}

ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    auto it = m_playerIndex.find(C_FCAST<libc_types::i32>(idx));

    if(it == m_playerIndex.end())
        return {};

    auto controller = C_RCAST<SDL_GameController*>(it->second);

    controller_map out;

    if constexpr(!compile_info::platform::is_emscripten)
    {
#define BTN SDL_GameControllerGetButton
#define AXIS SDL_GameControllerGetAxis

        out.buttons.e.a   = BTN(controller, SDL_CONTROLLER_BUTTON_A);
        out.buttons.e.b   = BTN(controller, SDL_CONTROLLER_BUTTON_B);
        out.buttons.e.x   = BTN(controller, SDL_CONTROLLER_BUTTON_X);
        out.buttons.e.y   = BTN(controller, SDL_CONTROLLER_BUTTON_Y);
        out.buttons.e.b_l = BTN(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        out.buttons.e.b_r =
            BTN(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        out.buttons.e.s_l  = BTN(controller, SDL_CONTROLLER_BUTTON_LEFTSTICK);
        out.buttons.e.s_r  = BTN(controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
        out.buttons.e.p_up = BTN(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
        out.buttons.e.p_down = BTN(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        out.buttons.e.p_left = BTN(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        out.buttons.e.p_right =
            BTN(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        out.buttons.e.back  = BTN(controller, SDL_CONTROLLER_BUTTON_BACK);
        out.buttons.e.start = BTN(controller, SDL_CONTROLLER_BUTTON_START);
        out.buttons.e.guide = BTN(controller, SDL_CONTROLLER_BUTTON_GUIDE);

        out.axes.e.l_x = rescale(AXIS(controller, SDL_CONTROLLER_AXIS_LEFTX));
        out.axes.e.l_y = rescale(AXIS(controller, SDL_CONTROLLER_AXIS_LEFTY));
        out.axes.e.r_x = rescale(AXIS(controller, SDL_CONTROLLER_AXIS_RIGHTX));
        out.axes.e.r_y = rescale(AXIS(controller, SDL_CONTROLLER_AXIS_RIGHTY));
        out.axes.e.t_l = AXIS(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        out.axes.e.t_r = AXIS(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

#undef BTN
#undef AXIS
    }

    return out;
}

comp_app::text_type_t ControllerInput::name(libc_types::u32 idx) const
{
    auto it = m_playerIndex.find(C_FCAST<libc_types::i32>(idx));

    if(it == m_playerIndex.end())
        return {};

    if constexpr(!compile_info::platform::is_emscripten)
    {
        auto name =
            SDL_GameControllerName(C_RCAST<SDL_GameController*>(it->second));

        return name ? name : CString();
    } else
        return CString();
}

libc_types::i16 ControllerInput::rescale(libc_types::i16 value) const
{
    if(value < 0)
    {
        if(value > -m_axisDeadzone)
            value = 0;
        else
            value += m_axisDeadzone;
    } else if(value > 0)
    {
        if(value < m_axisDeadzone)
            value = 0;
        else
            value -= m_axisDeadzone;
    }

    return libc_types::i16(m_axisScale * value);
}

int ControllerInput::controllerDisconnect(int device)
{
    auto it = m_controllers.find(device);

    if(it == m_controllers.end())
    {
        Throw(undefined_behavior("failed to close device"));
    }

    auto controller = C_RCAST<SDL_GameController*>(it->second);
    auto joystick   = SDL_GameControllerGetJoystick(controller);

    auto instanceIdx = SDL_JoystickInstanceID(joystick);

    SDL_GameControllerClose(controller);

    decltype(m_playerIndex)::key_type delete_key = -1;

    for(auto const& v : m_playerIndex)
        if(v.second == it->second)
            delete_key = v.first;

    m_playerIndex.erase(delete_key);
    m_controllers.erase(instanceIdx);

    return delete_key;
}

void KeyboardInput::start_restricted(proxy_type& p, time_point const&)
{
    using namespace Coffee::Input;

    CIEvent   inputEv;
    SDL_Event event;

    auto inputBus = get_container(p).service<comp_app::EventBus<CIEvent>>();

    while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP))
    {
        auto ev            = translate::event<CIKeyEvent>(event);
        m_register[ev.key] = ev.mod;
        EMIT_IEVENT(ev)
    }

    while(
        SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_TEXTEDITING, SDL_TEXTINPUT))
    {
        switch(event.type)
        {
        case SDL_TEXTEDITING: {
            CIWEditEvent edit;
            edit.cursor = event.edit.start;
            edit.len    = event.edit.length;
            EMIT_IEVENT(edit)
            break;
        }
        case SDL_TEXTINPUT: {
            CIWriteEvent write;
            write.text = event.text.text;
            EMIT_IEVENT(write)
            break;
        }
        default:
            break;
        }
    }
}

void KeyboardInput::openVirtual() const
{
    SDL_StartTextInput();
}

void KeyboardInput::closeVirtual() const
{
    SDL_StopTextInput();
}

void MouseInput::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;
    priority    = 256;
}

void MouseInput::start_restricted(proxy_type&, time_point const&)
{
    using namespace Coffee::Input;

    CIEvent inputEv;
    auto    inputBus = m_container->service<comp_app::EventBus<CIEvent>>();

    SDL_Event event;
    while(SDL_PeepEvents(
        &event, 1, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEWHEEL))
    {
        switch(event.type)
        {
        case SDL_MOUSEMOTION:
            EMIT_IEVENT(translate::event<CIMouseMoveEvent>(event))
            break;
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN: {
            auto ev = translate::event<CIMouseButtonEvent>(event);
            EMIT_IEVENT(ev)

            if(event.button.state == SDL_PRESSED)
                m_buttons |= ev.btn;
            else
                m_buttons &= (m_buttons ^ ev.btn);

            break;
        }
        case SDL_MOUSEWHEEL:
            EMIT_IEVENT(translate::event<CIScrollEvent>(event))
            break;
        }
    }
}

bool MouseInput::mouseGrabbed() const
{
    return SDL_GetRelativeMouseMode();
}

void MouseInput::setMouseGrab(bool enabled)
{
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

comp_app::position_t MouseInput::position() const
{
    position_t out;
    if(SDL_GetRelativeMouseMode())
        SDL_GetRelativeMouseState(&out.x, &out.y);
    else
        SDL_GetMouseState(&out.x, &out.y);

    return out;
}

void MouseInput::warp(const comp_app::position_t& newPos)
{
    SDL_WarpMouseInWindow(
        m_container->service<Windowing>()->m_window, newPos.x, newPos.y);
}

MouseInput::MouseButton MouseInput::buttons() const
{
    return m_buttons;
}

void WindowInfo::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;
}

comp_app::text_type_t WindowInfo::name() const
{
    auto name = SDL_GetWindowTitle(m_container->service<Windowing>()->m_window);

    if(name)
        return name;
    return {};
}

void WindowInfo::setName(comp_app::text_type newName)
{
    SDL_SetWindowTitle(
        m_container->service<Windowing>()->m_window, newName.c_str());
}

} // namespace sdl2

#include <SDL_syswm.h>

namespace sdl2 {

void getWindow(
    SDL_Window* window, comp_app::interfaces::PtrNativeWindowInfo& info)
{
    using namespace comp_app;

    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version)
    SDL_GetWindowWMInfo(window, &windowInfo);

#if defined(SDL_VIDEO_DRIVER_X11)
    info.window = C_RCAST<interfaces::PtrNativeWindowInfo::NWindow>(
        windowInfo.info.x11.window);
    info.display = windowInfo.info.x11.display;
#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
    info.window  = windowInfo.info.win.window;
    info.display = windowInfo.info.win.hdc;
#elif defined(SDL_VIDEO_DRIVER_COCOA)
    info.window  = windowInfo.info.cocoa.window;
    info.display = nullptr;
#elif defined(SDL_VIDEO_DRIVER_EMSCRIPTEN)
    /* There's nothing here? */
#elif defined(COFFEE_RASPBERRYPI)
    /* No data here */
#else
    static_assert(false, "missing video driver");
#endif
}

} // namespace sdl2
