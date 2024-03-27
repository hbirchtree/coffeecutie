#pragma once

#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/core/types/rect.h>
#include <peripherals/stl/types.h>

#if defined(FEATURE_ENABLE_ComponentApp)
#include <coffee/comp_app/services.h>
#else
#include "../base/renderer/windowapplication.h"
#include "../base/renderer/windowmanagerclient.h"
#endif

namespace Coffee::Display::EventHandlers {

using namespace Input;

struct WindowResize
{
    using event_type = ResizeEvent;

    WindowResize(std::function<void(u32, u32)>&& action)
        : m_action(std::move(action))
    {
    }

    void operator()(Event const&, ResizeEvent const* rev)
    {
        m_action(rev->w, rev->h);
    }

  private:
    std::function<void(u32, u32)> m_action;
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
    using event_type = typename std::conditional_t<
        std::is_same_v<Event, OnQuit>,
        Input::BaseEvent<CIEvent::QuitSign>,
        CIKeyEvent>;

    ExitOn(std::function<void()>&& action)
        : m_action(action)
    {
    }

    template<typename Event_ = Event>
    requires std::is_same_v<Event_, OnQuit>
    void operator()(CIEvent const& e, c_cptr)
    {
        if constexpr(compile_info::platform::is_emscripten)
            return;
        if(e.type != CIEvent::QuitSign)
            return;
        m_action();
    }

    template<typename Event_ = Event>
    requires(!std::is_same_v<Event, OnQuit>)
    void operator()(CIEvent const&, CIKeyEvent const* ev)
    {
        if constexpr(compile_info::platform::is_emscripten)
            return;
        if(ev->key != Event::key)
            return;
        m_action();
    }

  private:
    std::function<void()> m_action;
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

    FullscreenOn(
        std::function<void(bool)>&& action, std::function<bool()>&& getter)
        : m_action(std::move(action))
        , m_getter(std::move(getter))
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
    std::function<void(bool)> m_action;
    std::function<bool()>     m_getter;
};

} // namespace Coffee::Display::EventHandlers
