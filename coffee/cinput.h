#ifndef CINPUT_H
#define CINPUT_H

#include "coffee.h"

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
        Input        = 0x6,

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
        ShiftModifier     = 0x1,
        CtrlModifier      = 0x2,
        AltModifier       = 0x4,
        SuperModifier     = 0x8,
    };
    enum KeyEventType
    {
        Press      = 0x1,
        Release    = 0x2,
        Repeated   = 0x3,
    };
    uint8 type      = 0;

    uint32 keyCode  = 0;
    uint32 scanCode = 0;
    uint32 modifier = 0; // Modifier keys, flags
};

struct CITextEvent
{
    byte character  = 0;
};

struct CIMouseEvent
{
    enum MouseEventType
    {
        Press      = 0x1,
        Release    = 0x2,
        Move       = 0x3,

        Enter      = 0x4,
        Leave      = 0x5,
    };
    uint8 type = 0;

    uint8 modifier = 0;

    enum MouseButton
    {
        LeftButton     = 0x1,
        MiddleButton   = 0x2,
        RightButton    = 0x3,

        //buttons up to 255 are perfectly fine
    };
    uint8 keyCode = 0;
    CPointD pos; // Position where event occurred
    CPointD rel;
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
        SingleClick = 0x0,
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
    uint8 buttons         = 0;
    uint8 axes            = 0;
    uint8* buttonStates   = nullptr;
    scalar* axeStates       = nullptr;
};
struct CIJoyInfo
{
    CString name;
    uint8 buttons = 0;
    uint8 axes    = 0;
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

