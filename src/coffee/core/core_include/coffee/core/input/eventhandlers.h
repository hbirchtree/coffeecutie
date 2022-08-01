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

using namespace Input;

struct WindowResize
{
    using event_type = ResizeEvent;

    WindowResize(Function<void(u32, u32)>&& action) : m_action(std::move(action))
    {
    }

    void operator()(Event const&, ResizeEvent const* rev)
    {
        m_action(rev->w, rev->h);
    }

  private:
    Function<void(u32, u32)> m_action;
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
    using event_type = typename std::conditional<
        std::is_same<Event, OnQuit>::value,
        Input::BaseEvent<CIEvent::QuitSign>,
        CIKeyEvent>::type;

    ExitOn(Function<void()>&& action) : m_action(action)
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
        m_action();
    }

    template<
        typename Dummy = void,
        typename std::enable_if<!std::is_same<Event, OnQuit>::value, Dummy>::
            type* = nullptr>
    void operator()(CIEvent const&, CIKeyEvent const* ev)
    {
        if(ev->key != Event::key)
            return;
        m_action();
    }

  private:
    Function<void()> m_action;
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

    FullscreenOn(Function<void(bool)>&& action, Function<bool()>&& getter) : m_action(std::move(action)), m_getter(std::move(getter))
    {
    }

    void operator()(CIEvent const&, CIKeyEvent const* keyEvent)
    {
        if(Event::filter(*keyEvent))
        {
            if(m_getter())
                m_action(true);
            else
                m_action(false);
        }
    }

  private:
    Function<void(bool)> m_action;
    Function<bool()> m_getter;
};

} // namespace EventHandlers
} // namespace Display
} // namespace Coffee
