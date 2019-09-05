#include <coffee/sdl2_comp/app_components.h>

#include <SDL.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/entity_container.inl>
#include <peripherals/stl/string_casting.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include "sdl2events.h"

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

static const stl_types::MultiMap<Uint32, F> window_flag_mapping = {
    {SDL_WINDOW_SHOWN, F::Visible},

    {SDL_WINDOW_MINIMIZED, F::Minimized},
    {SDL_WINDOW_MAXIMIZED, F::Maximized},
    {SDL_WINDOW_RESIZABLE, F::Resizable},
    {SDL_WINDOW_BORDERLESS, F::Undecorated},

    {SDL_WINDOW_FULLSCREEN, F::FullScreen},
    {SDL_WINDOW_FULLSCREEN_DESKTOP, F::WindowedFullScreen}};

::Uint32 window_to_sdl2(comp_app::detail::WindowState state)
{
    Uint32 out = 0;

    for(auto const& v : window_flag_mapping)
        if(state & v.second)
            out |= v.first;

    return out;
}

libc_types::u16 window_from_sdl2(::Uint32 state)
{
    libc_types::u16 out = 0;

    for(auto const& v : window_flag_mapping)
        if(state & v.first)
            out |= v.second;

    return out;
}

using namespace stl_types;

void Context::load(entity_container& c, comp_app::app_error&)
{
    auto info = c.service<comp_app::AppInfo>();

    C_EXPECT_ZERO(SDL_Init(0));

    c.register_subsystem_services<comp_app::AppServiceTraits<Context>>(this);

    if(!info)
        return;

    SDL_version ver;
    SDL_GetVersion(&ver);

    auto verString = cast_pod(ver.major) + "." + cast_pod(ver.minor) + "." +
                     cast_pod(ver.patch);

    info->add("sdl2:version", verString);
}

void Context::unload(entity_container&, comp_app::app_error&)
{
    SDL_Quit();
}

void Context::end_restricted(Proxy& p, time_point const&)
{
    using namespace Coffee::Display;
    using namespace Coffee::Input;

    auto inputBus = p.service<comp_app::EventBus<CIEvent>>();

    C_PTR_CHECK_MSG(inputBus, "EventBus<CIEvent> not specified");

    CIEvent inputEv;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            EMIT_IEVENT(translate::event<CIKeyEvent>(event));
            break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            EMIT_IEVENT(translate::event<CIMouseButtonEvent>(event));
            break;

        case SDL_MOUSEMOTION:
            EMIT_IEVENT(translate::event<CIMouseMoveEvent>(event));
            break;

        case SDL_MOUSEWHEEL:
            EMIT_IEVENT(translate::event<CIScrollEvent>(event));
            break;

        case SDL_QUIT:
        {
            auto data    = translate::event<CIQuit>(event);
            inputEv.type = data.event_type;
            inputBus->process(inputEv, &data);

            m_shouldClose = data.shouldClose;
            break;
        }
        default:
            break;
        }

        inputEv = {};
    }
}

Windowing::~Windowing()
{
}

void Windowing::load(entity_container& c, comp_app::app_error& ec)
{
#if defined(COFFEE_LINUX)
    struct sigaction action;
    sigaction(SIGINT, nullptr, &action);
#endif

    if(NOT_ZERO(SDL_InitSubSystem(SDL_INIT_VIDEO)))
    {
        ec = comp_app::AppError::NoDisplay;
        ec = SDL_GetError();
        return;
    }

#if defined(COFFEE_LINUX)
    sigaction(SIGINT, &action, nullptr);
#endif

    auto& config = comp_app::AppLoader::config<comp_app::WindowConfig>(c);

    Uint32 extraFlags = SDL_WINDOW_ALLOW_HIGHDPI;

    if(c.service<sdl2::GLContext>())
        extraFlags |= SDL_WINDOW_OPENGL;
    else
        extraFlags |= SDL_WINDOW_VULKAN;

    c.service<GLContext>()->setupAttributes(c);

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
}

void Windowing::unload(entity_container&, comp_app::app_error&)
{
    if(m_window)
        SDL_DestroyWindow(m_window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Windowing::start_restricted(Proxy& p, time_point const&)
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

    while(SDL_PeepEvents(
        &event,
        1,
        SDL_eventaction::SDL_GETEVENT,
        SDL_WINDOWEVENT,
        SDL_WINDOWEVENT))
    {
        switch(event.window.type)
        {
        case SDL_WINDOWEVENT_ENTER:
        case SDL_WINDOWEVENT_LEAVE:
        case SDL_WINDOWEVENT_EXPOSED:
        case SDL_WINDOWEVENT_FOCUS_LOST:
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            EMIT_DEVENT(translate::event<FocusEvent>(event));
            break;
        case SDL_WINDOWEVENT_RESIZED:
            EMIT_DEVENT(translate::event<ResizeEvent>(event));
            break;
        case SDL_WINDOWEVENT_MOVED:
            EMIT_DEVENT(translate::event<MoveEvent>(event));
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

comp_app::size_2d_t DisplayInfo::physicalSize(libc_types::u32 idx) const
{
    return {};
}

void GLContext::setupAttributes(entity_container& c)
{
    using GLConfig = comp_app::GLConfig;
    using P        = typing::pixels::PixFmt;
    auto& glConfig = comp_app::AppLoader::config<GLConfig>(c);

    SDL_GL_SetAttribute(
        SDL_GL_FRAMEBUFFER_SRGB_CAPABLE,
        (glConfig.framebufferFmt == P::SRGB8 ||
         glConfig.framebufferFmt == P::SRGB8A8)
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
        Sint32 contextFlags = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;

#if defined(COFFEE_APPLE)
        contextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#endif

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

    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, color.a);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth_stencil.depth);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, depth_stencil.stencil);
}

template<typename version>
struct try_create_context
{
    void operator()(
        SDL_Window*                 window,
        SDL_GLContext*              m_context,
        comp_app::GLConfig::Profile profile)
    {
        if((profile & version::profile) == 0 || *m_context)
            return;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version::major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version::minor);

        *m_context = SDL_GL_CreateContext(window);

        if(*m_context)
            SDL_GetError();
    }
};

void GLContext::load(entity_container& c, comp_app::app_error& ec)
{
    using GLConfig = comp_app::GLConfig;

    auto  window   = c.service<Windowing>()->m_window;
    auto& glConfig = comp_app::AppLoader::config<GLConfig>(c);

    m_context = nullptr;

    type_safety::type_list::
        for_each_rev<GLConfig::valid_versions, try_create_context>(
            window, &m_context, glConfig.profile);

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
    }

    c.service<GLSwapControl>()->setSwapInterval(glConfig.swapInterval);

    m_container = &c;
}

void GLContext::unload(entity_container& c, comp_app::app_error& ec)
{
    if(m_context)
        SDL_GL_DeleteContext(m_context);
}

void GLContext::end_restricted(Proxy& p, time_point const&)
{
    comp_app::app_error ec;
    swapBuffers(ec);
    C_ERROR_CHECK(ec);
}

void GLContext::swapBuffers(comp_app::app_error& ec)
{
    SDL_GL_SwapWindow(m_container->service<sdl2::Windowing>()->m_window);
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

comp_app::size_2d_t GLFramebuffer::size() const
{
    size_2d_t out;
    SDL_GL_GetDrawableSize(
        m_container->service<Windowing>()->m_window, &out.w, &out.h);
    return out;
}

void ControllerInput::load(entity_container&, comp_app::app_error& ec)
{
    if(SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0)
    {
        ec = SDL_GetError();
        ec = comp_app::AppError::SystemError;
        return;
    }

    SDL_InitSubSystem(SDL_INIT_HAPTIC);
    SDL_GetError();
}

void ControllerInput::unload(entity_container&, comp_app::app_error&)
{
    for(auto controller : m_controllers)
        SDL_GameControllerClose(C_RCAST<SDL_GameController*>(controller));

    SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    SDL_QuitSubSystem(SDL_INIT_HAPTIC);
    SDL_GetError();
}

void ControllerInput::start_restricted(Proxy& p, time_point const&)
{
    using namespace Coffee::Input;

    auto    inputBus = get_container(p).service<comp_app::EventBus<CIEvent>>();
    CIEvent inputEv;

    SDL_Event event;

    while(SDL_PeepEvents(
        &event,
        1,
        SDL_GETEVENT,
        SDL_CONTROLLERDEVICEADDED,
        SDL_CONTROLLERDEVICEREMAPPED))
    {
        EMIT_IEVENT(translate::event<CIControllerAtomicUpdateEvent>(event));

        if(event.type == SDL_CONTROLLERDEVICEADDED)
        {
            auto controller = SDL_GameControllerOpen(event.cdevice.which);

            m_controllers.push_back(controller);
            SDL_GameControllerRumble(controller, 500, 4000, 2000);
        } else if(event.type == SDL_CONTROLLERDEVICEREMOVED)
        {
        }
    }

    SDL_GameControllerUpdate();
}

libc_types::u32 ControllerInput::count() const
{
    return C_FCAST<libc_types::u32>(m_controllers.size());
}

ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    if(idx >= m_controllers.size())
        return {};

    auto controller = C_RCAST<SDL_GameController*>(m_controllers.at(idx));

#define BTN SDL_GameControllerGetButton
#define AXIS SDL_GameControllerGetAxis

    controller_map out;

    out.buttons.e.a = BTN(controller, SDL_CONTROLLER_BUTTON_A);
    out.axes.e.l_x  = AXIS(controller, SDL_CONTROLLER_AXIS_LEFTX);
    out.axes.e.l_y  = AXIS(controller, SDL_CONTROLLER_AXIS_LEFTY);

#undef BTN
#undef AXIS

    return out;
}

comp_app::text_type_t ControllerInput::name(libc_types::u32 idx) const
{
    auto name = SDL_GameControllerName(
        C_RCAST<SDL_GameController*>(m_controllers.at(idx)));

    return name ? name : CString();
}

} // namespace sdl2
