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

#include "coffee.h"

struct Coffee::CInput::CIEvent
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

struct Coffee::CInput::CIKeyEvent
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
    uint8_t type = 0;

    uint32_t keyCode = 0;
    uint32_t modifier = 0; // Modifier keys
};
struct Coffee::CInput::CIMouseEvent
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
        NoButton       = 0b0,

        LeftButton     = 0b1,
        MiddleButton   = 0b10,
        RightButton    = 0b100,

        //buttons up to 255 are perfectly fine
    };
    uint8_t keyCode = 0; // 0 = no button, 1 = button 1 etc.
    double x = 0.f,y = 0.f; // Position where event occurred
};
struct Coffee::CInput::CIScrollEvent
{
    float deltaX = 0.f,deltaY = 0.f;
    uint8_t modifiers = 0;
};

struct Coffee::CInput::CIJoyState
{
    uint8_t buttons = 0;
    uint8_t axes = 0;
    uint8_t* buttonStates = nullptr;
    float* axeStates = nullptr;
};
struct Coffee::CInput::CIJoyInfo
{
    const char* name = nullptr;
    uint8_t buttons = 0;
    uint8_t axes = 0;
    float* axe_min = nullptr;
    float* axe_max = nullptr;
};

struct Coffee::CInput::CIDropEvent
{
    enum DataType
    {
        Unknown  = 0x0,

        Link     = 0x1,
        File     = 0x2,
    };

    uint8_t type = 0;
    uint32_t size = 0;
    void* data = nullptr;
};

class Coffee::CInput::CIEventParser : public QObject
{
    Q_OBJECT
public:
    CIEventParser(QObject* parent);

    static void createEvent(CIEvent::EventType i_type, const void* i_data, uint32_t i_size,
                     void* o_data, uint32_t &o_size);

public slots:
    void receiveEvent(void* data, uint16_t size);

signals:

};

#endif // COFFEEINPUTEVENT_H
