#ifndef CINPUT_H
#define CINPUT_H

#include "coffee.h"
#include "cinput_keymap.h"

namespace Coffee{
namespace CInput{

/*!
 * \brief Base for input events
 */
struct CIEvent
{
    enum EventType {
        MouseMove    = 0x1,
        Keyboard     = 0x2,
        Controller   = 0x3,
        Scroll       = 0x4,
        Drop         = 0x5, /*!< Drag-and-drop event*/
        TextInput    = 0x6,

        MouseButton  = 0x7,

        QuitSign     = 0x8, /*!< Notifies the program, can be ignored or handled*/

        Focus        = 0x9,
        TextEdit     = 0xa,

        ControllerEv = 0xb, /*!< Controller connection or disconnection*/
    };
    uint8   type  = 0; /*!< Event type*/
    uint32  ts    = 0; /*!< Event timestamp*/
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

    uint32 key  = 0;
    uint32 scan = 0; /*!< System scancode*/
    uint32 mod = 0; /*!< Modifier keys*/
};

/*!
 * \brief Text input event
 */
struct CITextEvent
{
    byte character  = 0;
};

/*!
 * \brief Mouse movement event
 */
struct CIMouseMoveEvent
{
    uint8 btn = 0;
    CPointF pos;
    CPointF rel;
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
    cstring  text   = nullptr;
};

/*!
 * \brief Text editing event, modifies a text input field
 */
struct CIWEditEvent
{
    cstring text    = nullptr;
    uint32  cursor  = 0;
    uint32  len     = 0;
};

/*!
 * \brief Full controller state dump
 */
struct CIControllerState
{
    uint8   id              = 0; /*!< Controller ID*/
    uint8   buttons         = 0; /*!< Amount of buttons*/
    uint8   axes            = 0; /*!< Amount of axes*/
    uint32  buttonstates    = 0; /*!< Button states as bits*/
    scalar* axeStates       = nullptr; /*!< Axis states as scalar values*/
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
        AxisMask        = 0x001, /*! Shifted 0, 1 bit*/
        ControllerMask  = 0x01e, /*! Shifted 1, 4 bits*/
        IndexMask       = 0x3e0, /*! Shifted 5, 5 bits*/
        ButtonStateMask = 0x400, /*! Shifted 10,1 bit*/
    };
    uint32  state   = 0; /*!< Stores most of state, whether it is an axis, which controller, which button or axis, and button state*/
    scalar  value   = 0.f; /*!< Scalar value for axis*/

    /*!
     * \brief Whether it is an axis or not
     * \return
     */
    bool axis() const
    {
        return state&AxisMask;
    }
    /*!
     * \brief Axis or button index
     * \return
     */
    byte index() const
    {
        return (state&IndexMask)>>5;
    }
    /*!
     * \brief Button state
     * \return
     */
    bool buttonState() const
    {
        return (state&ButtonStateMask)>>10;
    }
    /*!
     * \brief Controller index
     * \return
     */
    byte controller() const
    {
        return (state&ControllerMask)>>1;
    }
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

    uint32  state   = 0; /*!< Stores state, button count, axis count, controller index*/
    cstring name    = nullptr; /*!< Controller's name*/

    /*!
     * \brief Connected status
     * \return
     */
    bool connected() const
    {
        return (state&StateMask)&Connected;
    }
    /*!
     * \brief Remapping status
     * \return
     */
    bool remapped() const
    {
        return (state&StateMask)&Remapped;
    }
    /*!
     * \brief Count of buttons
     * \return
     */
    byte buttons() const
    {
        return (state&ButtonMask)>>2;
    }
    /*!
     * \brief Count of axes
     * \return
     */
    byte axes() const
    {
        return (state&AxisMask)>>7;
    }
    /*!
     * \brief Index of controller
     * \return
     */
    byte controller() const
    {
        return (state&ControllerMask)>>12;
    }
};

/*!
 * \brief Fat controller info
 */
struct CIControllerInfo
{
    /*!
     * \brief Controller state flags
     */
    enum ControllerFlags
    {
        Connected       = 0x1,
        Disconnected    = 0x2,
        Remapped        = 0x4,
    };

    cstring name    = nullptr; /*!< Controller name*/
    uint8   flags   = 0; /*!< Controller flags*/

    uint8   buttons = 0; /*!< Count of buttons*/
    uint8   axes    = 0; /*!< Count of axes*/
    scalar* axe_min = nullptr; /*!< Minimum axis values*/
    scalar* axe_max = nullptr; /*!< Maximum axis values*/
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

    uint8   type      = 0; /*!< Event type*/
    uint32  size      = 0; /*!< Size of data*/
    void*   data      = nullptr; /*!< Data pointer*/
};

/*!
 * \brief Sensor event, enumerated with ID, can provide uint64 value or bigscalar value.
 */
struct CISensorEvent
{
    uint64          id      = 0; /*!< Enumeration ID*/
    union {
        uint64      lvalue  = 0; /*!< Integer value for input*/
        bigscalar   dvalue; /*!< Floating-point value for input*/
    };
};

}
}



#endif // CINPUT_H

