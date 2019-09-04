#include <coffee/sdl2_comp/app_components.h>

#include <SDL.h>
#include <peripherals/stl/string_casting.h>

#include "sdl2events.h"

#define NOT_ZERO(v) (v < 0)

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

    c.register_subsystem_services<Context>(this);

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

    auto inputBus   = p.service<comp_app::EventBus<CIEvent>>();
    auto displayBus = p.service<comp_app::EventBus<Event>>();

    C_PTR_CHECK_MSG(inputBus, "EventBus<CIEvent> not specified");
    C_PTR_CHECK_MSG(displayBus, "EventBus<Display::Event> not specified");

#define EMIT_IEVENT(from)                  \
    {                                      \
        auto data    = from;               \
        inputEv.type = data.event_type;    \
        inputBus->process(inputEv, &data); \
    }
#define EMIT_DEVENT(from)                      \
    {                                          \
        auto data      = from;                 \
        displayEv.type = data.event_type;      \
        displayBus->process(displayEv, &data); \
    }

    CIEvent inputEv;
    Event   displayEv;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_WINDOWEVENT:
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
            break;
        }

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

        case SDL_QUIT:
        {
            auto data = translate::event<CIQuit>(event);
            inputEv.type = data.event_type;
            inputBus->process(inputEv, &data);

            m_shouldClose = data.shouldClose;
            break;
        }
        default:
            break;
        }

        inputEv   = {};
        displayEv = {};
    }

#undef EMIT_IEVENT
#undef EMIT_DEVENT
}

Windowing::~Windowing()
{
}

void Windowing::load(entity_container&, comp_app::app_error& ec)
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

    m_window = SDL_CreateWindow(nullptr, 0, 0, 500, 500, 0);

    if(!m_window)
    {
        ec = comp_app::AppError::NoWindow;
        ec = SDL_GetError();
        return;
    }
}

void Windowing::unload(entity_container&, comp_app::app_error&)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
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

} // namespace sdl2
