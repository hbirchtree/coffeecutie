#pragma once

#include "keymap.h"

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/vector_types.h>

/*!
 * \brief Input structures and enums, are commonly used by
 *  Display namespace
 */
namespace Coffee::Input {

using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;

constexpr szptr ci_max_text_edit_size = 32;

/*!
 * \brief Base for input events
 */
struct CIEvent
{
    enum EventType : u8
    {
        NoneType,

        MouseMove,   /*!< Mouse motion events*/
        MouseButton, /*!< Mouse button presses and releases*/
        Scroll,      /*!< Scroll events*/

        Keyboard, /*!< Keyboard events*/

        Controller,       /*!< CIControllerAtomicEvent */
        ControllerUpdate, /*!< CIControllerAtomicUpdateEvent */

        Drop, /*!< CIDropEvent */

        TextInput, /*!< Text input events*/
        TextEdit,  /*!< Text edit event, cursor position and stuff*/
        QuitSign,  /*!< Notifies the program, can be ignored or handled*/

        HapticDev, /*!< Sent when a new haptic device is connected or
                      disconnected*/
        Haptic,    /*!< Transmitted when user wants to create a rumble event*/

        TouchTap,    /*!< Single-finger tap */
        TouchMotion, /*!< Single-finger movement on screen */
        TouchPan,    /*!< Multi-touch events using several fingers*/
        TouchPinch,  /*!< Two-finger pinch */
        TouchRotate, /*!< Two-finger rotation */

        /* Deprecated, not really good descriptions */
        /* These originate from SDL */
        MultiTouch,
        Gesture,

        Sensor,
    };
    u32       ts   = 0;        /*!< Event timestamp*/
    EventType type = NoneType; /*!< Event type*/

    u8  pad1;
    u16 pad2;

    STATICINLINE CIEvent Create(u32 ts, EventType t)
    {
        CIEvent e;
        e.ts   = ts;
        e.type = t;
        return e;
    }
};

template<CIEvent::EventType Type>
struct BaseEvent
{
    using parent_type                              = CIEvent;
    static constexpr CIEvent::EventType event_type = Type;
};

struct CIQuit : BaseEvent<CIEvent::QuitSign>
{
    bool shouldClose = true;

    void deny()
    {
        shouldClose = false;
    }
};

/*!
 * \brief Keyboard events
 */
struct CIKeyEvent : BaseEvent<CIEvent::Keyboard>
{
    enum KeyModifiers : u16
    {
        NoneModifier = 0x0,

        LShiftModifier = 0x1,
        LCtrlModifier  = 0x2,
        LAltModifier   = 0x4,
        SuperModifier  = 0x8,

        RepeatedModifier = 0x10,
        PressedModifier  = 0x20,

        RShiftModifier = 0x040,
        RCtrlModifier  = 0x080,
        RAltModifier   = 0x100,

        CapsLockModifier = 0x200,
        NumLockModifier  = 0x400,
        AltGrModifier    = 0x800,
    };

    u32          key  = 0; /*!< A keycode, use this to identify keys*/
    u32          scan = 0; /*!< System scancode*/
    KeyModifiers mod  = NoneModifier; /*!< Modifier keys*/

    u16 pad1;
};

/*!
 * \brief Text input event
 */
struct CITextEvent : BaseEvent<CIEvent::TextInput>
{
    byte_t character = 0;
};

/*!
 * \brief Mouse movement event
 */
struct CIMouseMoveEvent : BaseEvent<CIEvent::MouseMove>
{
    CIMouseMoveEvent(Vecf2 o = {}, Vecf2 d = {}) : origin(o), delta(d)
    {
    }

    Vecf2 origin; /*!< Absolute position*/
    Vecf2 delta;  /*!< Relative movement since last poll*/
    union
    {
        u8   btn = 0; /*!< Button held down while moved*/
        bool hover;
    };

    u8  pad1;
    u16 pad2;
};

/*!
 * \brief Mouse button event
 */
struct CIMouseButtonEvent : BaseEvent<CIEvent::MouseButton>
{
    enum ButtonModifier : u8
    {
        NoneModifier = 0x0,

        DoubleClick = 0x1,
        Pressed     = 0x2,
    };
    enum MouseButton : u8
    {
        NoneBtn = 0x0,

        LeftButton   = 0x01,
        MiddleButton = 0x02,
        RightButton  = 0x04,
        X1Button     = 0x08,
        X2Button     = 0x10,

        AllButtons = 0x1F,
    };

    Vecf2          pos;                /*!< Mouse position*/
    ButtonModifier mod = NoneModifier; /*!< Modifier for event*/
    MouseButton    btn = NoneBtn;      /*!< Button*/

    u16 pad1;
};

/*!
 * \brief Mouse scroll event
 */
struct CIScrollEvent : BaseEvent<CIEvent::Scroll>
{
    CIScrollEvent(Vecf2 delta = {}, u8 mod = 0) : delta(delta), mod(mod)
    {
    }

    Vecf2 delta;   /*!< Delta for scroll*/
    u8    mod = 0; /*!< Current mouse modifiers*/

    u8  pad1;
    u16 pad2;
};

/*!
 * \brief Text input event, rich variant
 */
struct CIWriteEvent : BaseEvent<CIEvent::TextInput>
{
    cstring text; /*! Submitted text*/
};

/*!
 * \brief Text editing event, modifies a text input field
 */
struct CIWEditEvent : BaseEvent<CIEvent::TextEdit>
{
    i32     cursor; /*!< Cursor position*/
    i32     len;    /*!< Text length*/
    cstring text;   /*!< Current text*/
};

/*!
 * \brief Atomic input event type
 */
struct CIControllerAtomicEvent : BaseEvent<CIEvent::Controller>
{
    /*!
     * \brief Masks used to extract state
     */
    enum AtomicMasks
    {
        AxisMask        = 0x001, /*!< Shifted 0, 1 bit*/
        ControllerMask  = 0x01e, /*!< Shifted 1, 4 bits*/
        IndexMask       = 0x3e0, /*!< Shifted 5, 5 bits*/
        ButtonStateMask = 0x400, /*!< Shifted 10,1 bit*/
    };

    i16  value;            /*!< f32 value for axis*/
    u8   index : 5;        /*!< Button or axis index */
    u8   controller : 4;   /*!< Controller index */
    bool button_state : 1; /*!< when true, the button is pressed */
    bool axis : 1;         /*!< when true, this event is an axis event */

    bool is_button() const
    {
        return !axis;
    }

    bool is_axis() const
    {
        return axis;
    }

    u16 button_index() const
    {
        return CK_BUTTON_BASE + index;
    }

    u16 axis_index() const
    {
        return CK_AXIS_BASE + index;
    }

    template<typename T>
    T axis_value() const
    {
        return T(value);
    }
};

template<>
inline f32 CIControllerAtomicEvent::axis_value<f32>() const
{
    return C_CAST<f32>(value) / std::numeric_limits<i16>::max();
}

template<>
inline i16 CIControllerAtomicEvent::axis_value<i16>() const
{
    return value;
}

/*!
 * \brief Atomic controller disconnect and connect events
 */
struct CIControllerAtomicUpdateEvent : BaseEvent<CIEvent::ControllerUpdate>
{
    union
    {
        struct
        {
            u8   button : 5;
            u8   axis : 5;
            u8   controller : 4;
            bool connected : 1;
            bool remapped : 1;
        };
        u32 spacer;
    };
    u32 pad1;
};

struct CIControllerState
{
    union
    {
        struct
        {
            i16 l_x; /*!< Left X axis */
            i16 l_y; /*!< Left Y axis */
            i16 r_x; /*!< Right X axis */
            i16 r_y; /*!< Right Y axis */
            i16 t_l; /*!< Left trigger */
            i16 t_r; /*!< Right trigger */
        } e;
        i16 d[6];
    } axes;
    union
    {
        u16 d;
        struct
        {
            /* Buttons on right */
            bool a : 1;
            bool b : 1;
            bool x : 1;
            bool y : 1;

            /* Middle buttons */
            bool back : 1;
            bool guide : 1;
            bool start : 1;

            /* Stick buttons */
            bool s_l : 1;
            bool s_r : 1;

            /* Bumper buttons */
            bool b_l : 1;
            bool b_r : 1;

            /* D-pad buttons */
            bool p_up : 1;
            bool p_down : 1;
            bool p_left : 1;
            bool p_right : 1;
        } e;
    } buttons;
    u16 pad;
};

/*!
 * \brief Haptic events used for rumble.
 */
struct CIHapticEvent : BaseEvent<CIEvent::Haptic>
{
    union
    {
        u64 data = 0;
        struct
        {
            f32 strength;
            u32 duration : 24;
            u8  index;
        } rumble_input;
        struct
        {
            u8     index;
            byte_t name;
        } rumble_device;
    };
};

/*!
 * \brief Drag-and-drop event
 */
struct CIDropEvent : BaseEvent<CIEvent::Drop>
{
    /*!
     * \brief Data type for event
     */
    enum DataType : u8
    {
        Link = 0x1, /*!< Link to resource, internet or file*/
        File = 0x2, /*!< File path*/
        Text = 0x3, /*!< Text*/
    };

    union
    {
        void* data = 0;
        struct
        {
            cstring text;
        } text_data;
    };
    PACKEDSTRUCT({
        u32 size : 24; /*!< Size of data*/
        u8  type;      /*!< Event type*/
    });

    u32 pad1;
};

/*!
 * \brief Sensor event, enumerated with ID, can provide uint64 value or
 * f64 value.
 */
struct CISensorEvent : BaseEvent<CIEvent::Sensor>
{
    u64 id; /*!< Enumeration ID*/
    union
    {
        struct
        {
            i16 x;
            i16 y;
            i16 z;
            i16 w;
        } sivec;
        struct
        {
            u16 x;
            u16 y;
            u16 z;
            u16 w;
        } suvec;
        u64 lvalue; /*!< Integer value for input*/
        f64 dvalue; /*!< Floating-point value for input*/
    };
};

struct CITouchTapEvent : BaseEvent<CIEvent::TouchTap>
{
    enum TouchType
    {
        Pressed = 0x1,
    };
    Vecf2 pos;
    union
    {
        struct
        {
            u16  pressure;
            u16  finger : 15;
            bool pressed : 1;
        };
        u32 evdata;
    };
};

struct CITouchMotionEvent : BaseEvent<CIEvent::TouchMotion>
{
    Vecf2 origin;
    Vecf2 delta;
    union
    {
        struct
        {
            u16  pressure;
            u16  finger : 15;
            bool hover : 1;
        };
        u32 evdata;
    };
};

struct CIMTouchMotionEvent : BaseEvent<CIEvent::MultiTouch>
{
    Vecf2 origin;
    Vecf2 translation;
    Vecf2 velocity;
    union
    {
        struct
        {
            u16 fingers;
            i16 angle;
            i16 dist;
        };
        u64 evdata;
    };
};

struct CIGestureEvent : BaseEvent<CIEvent::Gesture>
{
    Vecf2 origin;
    union
    {
        struct
        {
            u32 gesture_id;
            u16 error;
            u16 fingers;
        };
        u64 evdata;
    };
};

struct CITouchPinchEvent : BaseEvent<CIEvent::TouchPinch>
{
    Vecf2 origin;
    f32   factor;
};

struct CITouchRotateEvent : BaseEvent<CIEvent::TouchRotate>
{
    Vecf2 origin;
    f32   radians;
};

C_FLAGS(CIKeyEvent::KeyModifiers, u16);
C_FLAGS(CIMouseButtonEvent::ButtonModifier, u8);
C_FLAGS(CIMouseButtonEvent::MouseButton, u8);

} // namespace Coffee::Input
