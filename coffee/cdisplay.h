#ifndef CDISPLAY_H
#define CDISPLAY_H

#include "coffee.h"

namespace Coffee{
namespace CDisplay{
struct CDColorSpace{
    uint8_t bits_red;
    uint8_t bits_green;
    uint8_t bits_blue;
};

struct CDMonitor{
    uint8_t         index;      //Real monitor index
    CDColorSpace    colorBits;  //Color depth
    CRect           screenArea; //Area occupied on screen
};
struct CDWindow{
    uintptr_t       handle;     //Native handle, in X11, the Window value,
    CString         title;      //Have a *wild* guess.
    CRect           screenArea;
};

struct CDEvent{
    enum EventType{
        //We will not use 0
        WindowRefreshed        = 0x1, //Perhaps not so interesting?
        WindowMoved            = 0x2,
        WindowFocusChanged     = 0x3,
        WindowResized          = 0x4,
        WindowStateChanged     = 0x5, //Includes "closed"
    };
    uint8_t type;
};

struct CDResizeEvent{
    uint32_t old_w;
    uint32_t old_h;

    uint32_t new_w;
    uint32_t new_h;
};

struct CDFocusEvent{
    bool gainedFocus;
};

struct CDStateEvent{
    enum StateChange{
        Minimized   = 0b1,
        Maximized   = 0b10,
        Closed      = 0b100,
        //TODO : Add more states here
    };
    uint8_t bits;
};

struct CDMoveEvent{
    uint32_t old_x;
    uint32_t old_y;

    uint32_t new_x;
    uint32_t new_y;
};

}
}

#endif // CDISPLAY_H

