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
        Joystick     = 0x3,
        Scroll       = 0x4,
        Drop         = 0x5, //Drag-and-drop
        TextInput    = 0x6,

        MouseButton    = 0x7,

        QuitSign     = 0x8, //Notifies the program
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
    uint8 modifier  = 0;
    uint8 btn       = 0;
};

struct CIScrollEvent
{
    CPointF delta;
    uint8 modifiers = 0;
};

struct CIWriteEvent
{
    uint32 character    = 0;
};

struct CIJoyState
{
    uint8   buttons         = 0;
    uint8   axes            = 0;
    uint8*  buttonStates    = nullptr;
    scalar* axeStates       = nullptr;
};
struct CIJoyInfo
{
    cstring name;
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

    uint8 type      = 0;
    uint32 size     = 0;
    void* data      = nullptr;
};

}
}



#endif // CINPUT_H

