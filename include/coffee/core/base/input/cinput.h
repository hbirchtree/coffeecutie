#ifndef CINPUT_H
#define CINPUT_H

#include "../../types/composite_types.h"
#include "../../types/vector_types.h"
#include "cinput_keymap.h"

namespace Coffee{
namespace CInput{

constexpr szptr ci_max_text_edit_size = 32;

/*!
 * \brief Base for input events
 */
struct CIEvent
{
    enum EventType : uint8 {
        NoneType     = 0x0,
        MouseMove    = 0x1,/*!< Mouse motion events*/
        Keyboard     = 0x2,/*!< Keyboard events*/
        Controller   = 0x3,/*!< Controller input events*/
        Scroll       = 0x4,/*!< Scroll events*/
        Drop         = 0x5,/*!< Drag-and-drop event*/
        TextInput    = 0x6,/*!< Text input events*/
        MouseButton  = 0x7,/*!< Mouse button presses and releases*/
        QuitSign     = 0x8,/*!< Notifies the program, can be ignored or handled*/
        TextEdit     = 0x9,/*!< Text edit event, cursor position and stuff*/
        ControllerUpdate = 0xa,/*!< Controller connection or disconnection*/
        HapticDev    = 0xb,/*!< Sent when a new haptic device is connected or disconnected*/
        Haptic       = 0xc,/*!< Transmitted when user wants to create a rumble event*/
    };
    uint32 ts = 0; /*!< Event timestamp*/
    EventType type = NoneType; /*!< Event type*/
};

/*!
 * \brief Keyboard events
 */
struct CIKeyEvent
{
    enum KeyModifiers : uint16
    {
        NoneModifier = 0x0,

        LShiftModifier    = 0x1,
        LCtrlModifier     = 0x2,
        LAltModifier      = 0x4,
        SuperModifier     = 0x8,

        RepeatedModifier  = 0x10,
        PressedModifier   = 0x20,

        RShiftModifier    = 0x040,
        RCtrlModifier     = 0x080,
        RAltModifier      = 0x100,

        CapsLockModifier  = 0x200,
        NumLockModifier   = 0x400,
        AltGrModifier     = 0x800,
    };

    uint32 key  = 0; /*!< A keycode, use this to identify keys*/
    uint32 scan = 0; /*!< System scancode*/
    KeyModifiers mod = NoneModifier; /*!< Modifier keys*/
};

/*!
 * \brief Text input event
 */
struct CITextEvent
{
    byte_t character  = 0;
};

/*!
 * \brief Mouse movement event
 */
struct CIMouseMoveEvent
{
    uint8 btn = 0; /*!< Button held down while moved*/
    CPointF pos; /*!< Absolute position*/
    CPointF rel; /*!< Relative movement since last poll*/
};

/*!
 * \brief Mouse button event
 */
struct CIMouseButtonEvent
{
    enum ButtonModifier : uint8
    {
        NoneModifier = 0x0,

        DoubleClick = 0x1,
        Pressed     = 0x2,
    };
    enum MouseButton : uint8
    {
        NoneBtn = 0x0,

        LeftButton      = 0x01,
        MiddleButton    = 0x02,
        RightButton     = 0x04,
        X1Button        = 0x08,
        X2Button        = 0x10,
    };

    CPointF pos; /*!< Mouse position*/
    ButtonModifier mod = NoneModifier; /*!< Modifier for event*/
    MouseButton btn = NoneBtn; /*!< Button*/
};

/*!
 * \brief Mouse scroll event
 */
struct CIScrollEvent
{
    CPointF delta; /*!< Delta for scroll*/
    uint8 mod = 0; /*!< Current mouse modifiers*/
};

/*!
 * \brief Text input event, rich variant
 */
struct CIWriteEvent
{
    cstring text; /*! Submitted text*/
};

/*!
 * \brief Text editing event, modifies a text input field
 */
struct CIWEditEvent
{
    int32 cursor; /*!< Cursor position*/
    int32 len; /*!< Text length*/
    cstring text; /*!< Current text*/
};

/*!
 * \brief Atomic input event type
 */
struct CIControllerAtomicEvent
{
    /*!
     * \brief Masks used to extract state
     */
    enum AtomicMasks{
        AxisMask        = 0x001, /*!< Shifted 0, 1 bit*/
        ControllerMask  = 0x01e, /*!< Shifted 1, 4 bits*/
        IndexMask       = 0x3e0, /*!< Shifted 5, 5 bits*/
        ButtonStateMask = 0x400, /*!< Shifted 10,1 bit*/
    };

    int16 value; /*!< Scalar value for axis*/
    uint8 index:5;
    uint8 controller:4;
    bool button_state:1;
    bool axis:1;
};

/*!
 * \brief Atomic controller disconnect and connect events
 */
struct CIControllerAtomicUpdateEvent
{
    uint8 button:5;
    uint8 axis:5;
    uint8 controller:4;
    bool connected:1;
    bool remapped:1;
    cstring name;
};

struct CIControllerState
{
    union{
        struct{
            int16 l_x;
            int16 l_y;
            int16 r_x;
            int16 r_y;
            int16 t_l;
            int16 t_r;
        } e;
        int16 d[6];
    } axes;
    union{
        uint16 d;
        struct{
            bool a:1;
            bool b:1;
            bool x:1;
            bool y:1;

            bool back:1;
            bool guide:1;
            bool start:1;

            bool s_l:1;
            bool s_r:1;

            bool b_l:1;
            bool b_r:1;

            bool p_up:1;
            bool p_down:1;
            bool p_left:1;
            bool p_right:1;
        } e;
    } buttons;
};

/*!
 * \brief Haptic events used for rumble.
 */
struct CIHapticEvent
{
    union {
        uint64 data = 0;
        struct
        {
            scalar strength;
            uint32 duration:24;
            uint8 index;
        } rumble_input;
        struct
        {
            uint8 index;
            byte_t name[];
        } rumble_device;
    };
};

/*!
 * \brief Drag-and-drop event
 */
struct CIDropEvent
{
    /*!
     * \brief Data type for event
     */
    enum DataType : uint8
    {
        Link     = 0x1, /*!< Link to resource, internet or file*/
        File     = 0x2, /*!< File path*/
        Text     = 0x3, /*!< Text*/
    };

    uint32  size:24; /*!< Size of data*/
    DataType type; /*!< Event type*/
    union{
        void* data = 0;
        struct
        {
            cstring text;
        } text_data;
    };
};

/*!
 * \brief Sensor event, enumerated with ID, can provide uint64 value or bigscalar value.
 */
struct CISensorEvent
{
    uint64 id; /*!< Enumeration ID*/
    union {
        struct{
            int16 x;
            int16 y;
            int16 z;
            int16 w;
        } sivec;
        struct{
            uint16 x;
            uint16 y;
            uint16 z;
            uint16 w;
        } suvec;
        uint64      lvalue; /*!< Integer value for input*/
        bigscalar   dvalue; /*!< Floating-point value for input*/
    };
};

C_FLAGS(CIKeyEvent::KeyModifiers,uint16);
C_FLAGS(CIMouseButtonEvent::ButtonModifier,uint8);
C_FLAGS(CIMouseButtonEvent::MouseButton,uint8);

}
}



#endif // CINPUT_H

