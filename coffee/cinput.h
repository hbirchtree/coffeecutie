#ifndef CINPUT_H
#define CINPUT_H

#include "coffee.h"

namespace Coffee{
namespace CInput{
struct CIEvent
{
    enum EventType {
        Mouse        = 0x1,
        Keyboard     = 0x2,
        Joystick     = 0x3,
        Scroll       = 0x4,
        Drop         = 0x5, //Drag-and-drop
        Input        = 0x6,
    };
    uint8 type = 0;
};

struct CIKeyEvent
{
    enum KeyModifiers
    {
        ShiftModifier     = 0b1,
        CtrlModifier      = 0b10,
        AltModifier       = 0b100,
        SuperModifier     = 0b1000,
    };
    enum KeyEventType
    {
        Press      = 0x1,
        Release    = 0x2,
        Repeated   = 0x3,

        Text       = 0x4,
    };
    uint8 type = 0;

    uint32 keyCode  = 0;
    uint32 scanCode = 0;
    uint32 modifier = 0; // Modifier keys, flags
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
    bigscalar x = 0.f,y = 0.f; // Position where event occurred
};
struct CIScrollEvent
{
    scalar deltaX = 0.f,deltaY = 0.f;
    uint8 modifiers = 0;
};

struct CIWriteEvent
{
    uint32 character;
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

    uint8 type    = 0;
    uint32 size   = 0;
    void* data      = nullptr;
};

}
}



#endif // CINPUT_H

