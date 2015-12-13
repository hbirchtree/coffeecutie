#ifndef CINPUT_H
#define CINPUT_H

#include "coffee/core/coffee.h"
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
    enum KeyModifiers
    {
        LShiftModifier    = 0x1,
        LCtrlModifier     = 0x2,
        LAltModifier      = 0x4,
        SuperModifier     = 0x8,

        RShiftModifier    = 0x040,
        RCtrlModifier     = 0x080,
        RAltModifier      = 0x100,

        CapsLockModifier  = 0x200,
        NumLockModifier   = 0x200,
        AltGrModifier     = 0x400,

        RepeatedModifier  = 0x10,
        PressedModifier   = 0x20,
    };

    uint32 key  = 0; /*!< A keycode, use this to identify keys*/
    uint32 scan = 0; /*!< System scancode*/
    uint32 mod = 0; /*!< Modifier keys*/
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
    enum ButtonModifier
    {
        DoubleClick = 0x1,
        Pressed     = 0x2,
    };
    enum MouseButton
    {
        LeftButton      = 0x01,
        MiddleButton    = 0x02,
        RightButton     = 0x04,
        X1Button        = 0x08,
        X2Button        = 0x10,
    };

    CPointF pos; /*!< Mouse position*/
    uint8 mod       = 0; /*!< Modifier for event*/
    uint8 btn       = 0; /*!< Button*/
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
    /*!
     * \brief Masks used to extract state
     */
    enum Masks
    {
        StateMask       = 0x0003, /*!< Shifted 0,  2 bits*/
        ButtonMask      = 0x007c, /*!< Shifted 2,  5 bits*/
        AxisMask        = 0x0f80, /*!< Shifted 7,  5 bits*/
        ControllerMask  = 0xf000, /*!< Shifted 12, 4 bits*/
    };
    /*!
     * \brief State for controller
     */
    enum States
    {
        Connected   = 0x1,
        Remapped    = 0x2,
    };

    uint8 button:5;
    uint8 axis:5;
    uint8 controller:4;
    bool connected:1;
    bool remapped:1;
    cstring name;
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
    enum DataType
    {
        Link     = 0x1, /*!< Link to resource, internet or file*/
        File     = 0x2, /*!< File path*/
        Text     = 0x3, /*!< Text*/
    };

    uint32  size:24; /*!< Size of data*/
    uint8   type; /*!< Event type*/
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

}
}



#endif // CINPUT_H

