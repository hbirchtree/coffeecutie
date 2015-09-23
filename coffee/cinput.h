#ifndef CINPUT_H
#define CINPUT_H

#include "coffee.h"
#include "cinput_keymap.h"

namespace Coffee{
namespace CInput{

struct CIEvent
{
    enum EventType {
        MouseMove    = 0x1,
        Keyboard     = 0x2,
        Controller   = 0x3,
        Scroll       = 0x4,
        Drop         = 0x5, //Drag-and-drop
        TextInput    = 0x6,

        MouseButton  = 0x7,

        QuitSign     = 0x8, //Notifies the program

        Focus        = 0x9,
        TextEdit     = 0xa,

        ControllerEv = 0xb,
    };
    uint8   type  = 0;
    uint32  ts    = 0;
};

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
    uint32 scan = 0;
    uint32 mod = 0; // Modifier keys, flags
};

struct CITextEvent
{
    byte character  = 0;
};

struct CIMouseMoveEvent
{
    uint8 btn = 0;
    CPointF pos;
    CPointF rel;
};

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

    CPointF pos;
    uint8 mod  = 0;
    uint8 btn       = 0;
};

struct CIScrollEvent
{
    CPointF delta;
    uint8 mod = 0;
};

struct CIFocusEvent
{
    enum FocusModifier
    {
        Enter   = 0x1,
        Leave   = 0x2,
    };

    uint8 mod = 0;
};

struct CIWriteEvent
{
    cstring  text   = nullptr;
};

struct CIWEditEvent
{
    cstring text    = nullptr;
    uint32  cursor  = 0;
    uint32  len     = 0;
};

struct CIControllerState
{
    uint8   id              = 0;
    uint8   buttons         = 0;
    uint8   axes            = 0;
    uint32  buttonstates    = 0;
    scalar* axeStates       = nullptr;
};

struct CIControllerAtomicEvent
{
    enum AtomicMasks{
        AxisMask        = 0x001, //Shifted 0, 1 bit
        ControllerMask  = 0x01e, //Shifted 1, 4 bits
        IndexMask       = 0x3e0, //Shifted 5, 5 bits
        ButtonStateMask = 0x400, //Shifted 10, 1 bit
    };
    uint32  state   = 0;
    scalar  value   = 0.f;
};

struct CIControllerAtomicUpdateEvent
{
    enum Masks
    {
        StateMask       = 0x0003, //Shifted 0,  2 bits
        ButtonMask      = 0x007c, //Shifted 2,  5 bits
        AxisMask        = 0x0f80, //Shifted 7,  5 bits
        ControllerMask  = 0xf000, //Shifted 12, 4 bits
    };
    enum States
    {
        Connected   = 0x1,
        Remapped    = 0x2,
    };

    uint32  state   = 0;
    cstring name    = nullptr;
};

struct CIControllerInfo
{
    enum ControllerFlags
    {
        Connected       = 0x1,
        Disconnected    = 0x2,
        Remapped        = 0x4,
    };

    cstring name    = nullptr;
    uint8   flags   = 0;

    uint8   buttons = 0;
    uint8   axes    = 0;
    scalar* axe_min = nullptr;
    scalar* axe_max = nullptr;
};


struct CIDropEvent
{
    enum DataType
    {
        Link     = 0x1,
        File     = 0x2,
        Text     = 0x3,
    };

    uint8   type      = 0;
    uint32  size      = 0;
    void*   data      = nullptr;
};

struct CISensorEvent
{
    uint64          id      = 0;
    union {
        uint64      lvalue  = 0;
        bigscalar   dvalue;
    };
};

}
}



#endif // CINPUT_H

