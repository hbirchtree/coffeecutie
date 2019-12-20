#pragma once

#include <coffee/core/stl_types.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/core/types/rect.h>

#if defined(FEATURE_ENABLE_ComponentApp)
#include <coffee/comp_app/services.h>
#else
#include "../base/renderer/windowapplication.h"
#include "../base/renderer/windowmanagerclient.h"
#endif

namespace Coffee {
namespace Display {
namespace EventHandlers {

namespace detail {

#if defined(FEATURE_ENABLE_ComponentApp)
void closeWindow(comp_app::Windowing* window)
{
    window->close();
}
void setState(comp_app::Windowing* window, Properties::State state)
{
    window->setState(state);
}
Properties::State getState(comp_app::Windowing* window)
{
    return window->state();
}

using WindowPtr    = comp_app::detail::ServiceRef<comp_app::Windowing>;
using WindowManPtr = comp_app::detail::ServiceRef<comp_app::Windowing>;
#else
void closeWindow(WindowApplication* window)
{
    window->closeWindow();
}
void setState(WindowManagerClient* window, Properties::State state)
{
    window->setWindowState(state);
}
u32 getState(WindowManagerClient* window)
{
    return window->windowState();
}

using WindowPtr    = WkPtr<WindowApplication>;
using WindowManPtr = WkPtr<WindowManagerClient>;
#endif

} // namespace detail

using namespace Input;

template<class GLM>
struct WindowResize
{
    using event_type = ResizeEvent;

    WindowResize()
    {
    }

    void operator()(Event const&, ResizeEvent const* rev)
    {
        Rect64 view(0, 0, rev->w, rev->h);
        GLM::DefaultFramebuffer()->resize(0, view);
    }
};

struct OnQuit
{
};

template<InputCode Key>
struct OnKey
{
    static constexpr InputCode key = Key;
};

template<typename Event, typename WindowAppPtr = detail::WindowPtr>
struct ExitOn
{
    using event_type = typename std::conditional<
        std::is_same<Event, OnQuit>::value,
        Input::BaseEvent<CIEvent::QuitSign>,
        CIKeyEvent>::type;

    ExitOn(WindowAppPtr renderer) : m_renderer(renderer)
    {
    }

    template<
        typename Dummy = void,
        typename std::enable_if<std::is_same<Event, OnQuit>::value, Dummy>::
            type* = nullptr>
    void operator()(CIEvent const& e, c_cptr)
    {
        if(e.type != CIEvent::QuitSign)
            return;

        auto ptr = m_renderer.lock();
        C_PTR_CHECK(ptr);
        detail::closeWindow(ptr);
    }

    template<
        typename Dummy = void,
        typename std::enable_if<!std::is_same<Event, OnQuit>::value, Dummy>::
            type* = nullptr>
    void operator()(CIEvent const&, CIKeyEvent const* ev)
    {
        if(ev->key != Event::key)
            return;

        auto ptr = m_renderer.lock();
        C_PTR_CHECK(ptr);
        detail::closeWindow(ptr);
    }

  private:
    WindowAppPtr m_renderer;
};

template<
    InputCode                Key,
    CIKeyEvent::KeyModifiers Mods = CIKeyEvent::KeyModifiers::NoneModifier>
struct KeyCombo
{
    static constexpr InputCode                key       = Key;
    static constexpr CIKeyEvent::KeyModifiers modifiers = Mods;
};

namespace detail {

template<class K>
STATICINLINE bool check_key(CIKeyEvent const& keyEvent)
{
    constexpr auto mods = K::modifiers;

    return (keyEvent.mod & (CIKeyEvent::PressedModifier |
                            CIKeyEvent::RepeatedModifier)) == 0 &&
           keyEvent.key == K::key && (keyEvent.mod & mods) == mods;
}

template<
    class K,
    class... KeyCombos,
    typename std::enable_if<sizeof...(KeyCombos) >= 1>::type* = nullptr>
STATICINLINE bool check_key(CIKeyEvent const& keyEvent)
{
    return check_key<K>(keyEvent) || check_key<KeyCombos...>(keyEvent);
}

} // namespace detail

template<class... KeyCombo>
struct AnyIKey
{
    STATICINLINE bool filter(CIKeyEvent const& keyEvent)
    {
        return detail::check_key<KeyCombo...>(keyEvent);
    }
};

template<typename Event, typename WindowManPtr = detail::WindowManPtr>
struct FullscreenOn
{
    using event_type = CIKeyEvent;

    FullscreenOn(WindowManPtr renderer) : m_renderer(renderer)
    {
    }

    void operator()(CIEvent const&, CIKeyEvent const* keyEvent)
    {
        if(Event::filter(*keyEvent))
        {
            if(detail::getState(m_renderer) & Properties::Windowed)
                detail::setState(m_renderer, Properties::WindowedFullScreen);
            else
                detail::setState(m_renderer, Properties::Windowed);
        }
    }

  private:
    WindowManPtr m_renderer;
};

} // namespace EventHandlers
} // namespace Display
} // namespace Coffee
