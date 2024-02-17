#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/size.h>
#include <peripherals/enum/helpers.h>

namespace Coffee {
namespace Display {

/*!
 * \brief Window event
 */
struct Event
{
    enum EventType : u8
    {
        Refresh = 1,
        Move,
        Resize,
        State,

        Focus,

        IsBackground,
        IsForeground,

        TransitionBackground,
        TransitionForeground,
    };

    u32       ts{0};  /*!< Event timestamp*/
    EventType type{}; /*!< Event type*/

    STATICINLINE Event Create(u32 ts, EventType et)
    {
        return {ts, et};
    }
};

template<Event::EventType Type>
struct BaseEvent
{
    using parent_type                            = Event;
    static constexpr Event::EventType event_type = Type;
};

/*!
 * \brief Window state event
 */
struct StateEvent : BaseEvent<Event::State>
{
    enum StateChange : u8
    {
        Minimized = 0x01,
        Maximized = 0x02,
        Restored  = 0x03,

        Closed = 0x04,

        Hidden = 0x05,
        Shown  = 0x06,
    };

    StateChange type{0}; /*!< Type of window state event*/

    StateEvent(StateChange type)
        : type(type)
    {
    }
};

/*!
 * \brief Window focus event
 */
struct FocusEvent : BaseEvent<Event::Focus>
{
    enum FocusMask : u8
    {
        None    = 0x0,
        Mouse   = 0x1,
        Enter   = 0x2,
        Exposed = 0x4,
        Leave   = 0x8,
    };

    FocusMask mod{None}; /*!< Type of focus event*/

    FocusEvent(FocusMask mask = None)
        : mod(mask)
    {
    }
};

C_FLAGS(FocusEvent::FocusMask, u8);

struct MoveEvent
    : Point
    , BaseEvent<Event::Move>
{
    using Point::point_2d;

    MoveEvent(i32 x = 0, i32 y = 0)
    {
        this->x = x;
        this->y = y;
    }
};

struct ResizeEvent
    : Size
    , BaseEvent<Event::Resize>
{
    using Size::size_2d;

    ResizeEvent(i32 w = 0, i32 h = 0)
    {
        this->w = w;
        this->h = h;
    }
};

} // namespace Display
} // namespace Coffee
