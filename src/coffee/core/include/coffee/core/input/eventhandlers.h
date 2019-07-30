#pragma once

#include <coffee/core/stl_types.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/core/types/rect.h>

#include "../base/renderer/windowapplication.h"
#include "../base/renderer/windowmanagerclient.h"

namespace Coffee {
namespace Display {
namespace EventHandlers {

using namespace Input;

template<class GLM>
struct WindowResize
{
    using event_type = ResizeEvent;

    WindowResize()
    {
    }

    void operator()(Event const& e, ResizeEvent const* rev) const
    {
        Rect64 view(0, 0, rev->w, rev->h);
        GLM::DefaultFramebuffer().resize(0, view);
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

template<typename Event>
struct ExitOn
{
    using event_type = CIKeyEvent;

    ExitOn(ShPtr<WindowApplication> const& renderer) : m_renderer(renderer)
    {
    }

    template<
        typename Dummy = void,
        typename std::enable_if<std::is_same<Event, OnQuit>::value, Dummy>::
            type* = nullptr>
    void operator()(CIEvent const& e, c_cptr) const
    {
        if(e.type != CIEvent::QuitSign)
            return;

        auto ptr = m_renderer.lock();

        if(!ptr)
            Throw(undefined_behavior("expired pointer"));

        ptr->closeWindow();
    }

    template<
        typename Dummy = void,
        typename std::enable_if<!std::is_same<Event, OnQuit>::value, Dummy>::
            type* = nullptr>
    void operator()(CIEvent const&, CIKeyEvent const* ev) const
    {
        if(ev->key != Event::key)
            return;

        auto ptr = m_renderer.lock();

        if(!ptr)
            Throw(undefined_behavior("expired pointer"));

        ptr->closeWindow();
    }

  private:
    WkPtr<WindowApplication> m_renderer;
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

template<typename Event>
struct FullscreenOn
{
    using event_type = CIKeyEvent;

    FullscreenOn(ShPtr<WindowManagerClient> const& renderer) :
        m_renderer(renderer)
    {
    }

    void operator()(CIEvent const&, CIKeyEvent const* keyEvent) const
    {
        if(Event::filter(*keyEvent))
        {
            if(m_renderer->windowState() & Properties::Windowed)
                m_renderer->setWindowState(Properties::WindowedFullScreen);
            else
                m_renderer->setWindowState(Properties::Windowed);
        }
    }

  private:
    ShPtr<WindowManagerClient> m_renderer;
};

} // namespace EventHandlers
} // namespace Display
} // namespace Coffee
