#ifndef COFFEEINPUTEVENT_H
#define COFFEEINPUTEVENT_H

#include <QObject>
#include "general/common.h"

//Hex codes are only for simplicity when looking them up

/*
 * The basic jist of it:
 *  - A data packet is sent from CoffeeRenderer to script environment
 *  - Packet contains the CoffeeInputEvent structure as a header, describing what the event is
 *  - The parser extracts the information it needs from the event and creates signals
 *
*/

struct CoffeeInputEvent
{
    enum EventType {
        NoEvent      = 0x0,

        Mouse        = 0x1,
        Keyboard     = 0x2,
        Joystick     = 0x3,
        Scroll       = 0x4,
        Drop         = 0x5, //Drag-and-drop
    };
     uint8_t type = 0;
};

struct CoffeeKeyEvent
{
    enum KeyEventType
    {
        NoEvent    = 0x0,

        Press      = 0x1,
        Release    = 0x2,
        Repeated   = 0x3,

        Text       = 0x4,
    };
    uint8_t type = 0;

    uint32_t keyCode = 0;
    uint32_t modifier = 0; // Modifier keys
};
struct CoffeeMouseEvent
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
    uint8_t type = 0;

    uint8_t modifier = 0;

    enum MouseButton
    {
        NoButton       = 0x0,

        LeftButton     = 0x1,
        MiddleButton   = 0x2,
        RightButton    = 0x3,

        //buttons up to 255 are perfectly fine
    };
    uint8_t keyCode = 0; // 0 = no button, 1 = button 1 etc.
    float x = 0.f,y = 0.f; // Position where event occurred
};
struct CoffeeScrollEvent
{
    float deltaX = 0.f,deltaY = 0.f;
    uint8_t modifiers = 0;
};
struct CoffeeJoystickState
{
    uint8_t buttons = 0;
    uint8_t axes = 0;
    uint8_t* buttonStates = nullptr;
    float* axeStates = nullptr;
};
struct CoffeeDropEvent
{
    enum DataType
    {
        Unknown  = 0x0,

        Link     = 0x01,
        File     = 0x02,
    };

    uint8_t type = 0;
    uint32_t size = 0;
    void* data = nullptr;
};

class CoffeeInputEventParser : public QObject
{
    Q_OBJECT
};

#endif // COFFEEINPUTEVENT_H
