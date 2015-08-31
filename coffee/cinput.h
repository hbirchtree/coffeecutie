#ifndef CINPUT_H
#define CINPUT_H

#include "coffee.h"

namespace Coffee{
namespace CInput{
struct CIEvent
{
    enum EventType {
        NoEvent      = 0x0,

        Mouse        = 0x1,
        Keyboard     = 0x2,
        Joystick     = 0x3,
        Scroll       = 0x4,
        Drop         = 0x5, //Drag-and-drop
    };
    uint8_t type = NoEvent;
};

struct CIKeyEvent
{
    enum KeyModifiers
    {
        NoModifier        = 0,

        ShiftModifier     = 0b1,
        CtrlModifier      = 0b10,
        AltModifier       = 0b100,
        SuperModifier     = 0b1000,
    };
    enum KeyEventType
    {
        NoEvent    = 0x0,

        Press      = 0x1,
        Release    = 0x2,
        Repeated   = 0x3,

        Text       = 0x4,
    };
    uint8_t type = NoEvent;

    uint32_t keyCode = 0;
    uint32_t modifier = NoModifier; // Modifier keys, flags
};
struct CIMouseEvent
{
    enum MouseEventType
    {
        NoEvent    = 0x0,

        Press      = 0x1,
        Release    = 0x2,
        Move       = 0x3,

        Enter      = 0x4,
        Leave      = 0x5,
    };
    uint8_t type = NoEvent;

    uint8_t modifier = CIKeyEvent::NoModifier;

    enum MouseButton
    {
        NoButton       = 0x0,

        LeftButton     = 0x1,
        MiddleButton   = 0x2,
        RightButton    = 0x3,

        //buttons up to 255 are perfectly fine
    };
    uint8_t keyCode = NoButton;
    double x = 0.f,y = 0.f; // Position where event occurred
};
struct CIScrollEvent
{
    float deltaX = 0.f,deltaY = 0.f;
    uint8_t modifiers = 0;
};

struct CIJoyState
{
    uint8_t buttons = 0;
    uint8_t axes = 0;
    uint8_t* buttonStates = nullptr;
    float* axeStates = nullptr;
};
struct CIJoyInfo
{
    CString name;
    uint8_t buttons = 0;
    uint8_t axes = 0;
    float* axe_min = nullptr;
    float* axe_max = nullptr;
};

struct CIDropEvent
{
    enum DataType
    {
        Unknown  = 0x0,

        Link     = 0x1,
        File     = 0x2,
        Text     = 0x3,
    };

    uint8_t type = Unknown;
    uint32_t size = 0;
    void* data = nullptr;
};

}
}



#endif // CINPUT_H

